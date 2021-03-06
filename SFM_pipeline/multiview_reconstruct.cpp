//LSM Gautham J.S OCT-2020
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>


#include <gtsam/geometry/Point2.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/ProjectionFactor.h>
#include <gtsam/slam/GeneralSFMFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/nonlinear/Values.h>

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cassert>

#include "meta.h"


using namespace std;
using namespace cv;

const std::string IMAGE_DIR = "";
const std::string img_dir3 = "/home/gautham/Documents/Datasets/Statue2/*.jpg";
const std::string img_dir2 = "/home/gautham/Documents/Datasets/SfM_quality_evaluation/Benchmarking_Camera_Calibration_2008/fountain-P11/images/Testing/*.jpg";


std::vector<std::string> IMAGES;


vector<cv::String> impath;

struct SFM_metadata{
    struct ImagePose{
        cv::Mat img; 
        cv::Mat desc;
        std::vector<cv::KeyPoint> kp;

        cv::Mat T;
        cv::Mat P;

        using kp_idx_t = size_t;
        using landmark_idx_t = size_t;
        using img_idx_t = size_t;

        std::map<kp_idx_t, std::map<img_idx_t, kp_idx_t>> kp_matches; 
        std::map<kp_idx_t, landmark_idx_t> kp_landmark;

        kp_idx_t& kp_match_idx(size_t kp_idx, size_t img_idx) { return kp_matches[kp_idx][img_idx]; };
        bool kp_match_exist(size_t kp_idx, size_t img_idx) { return kp_matches[kp_idx].count(img_idx) > 0; };

        landmark_idx_t& kp_3d(size_t kp_idx) { return kp_landmark[kp_idx]; }
        bool kp_3d_exist(size_t kp_idx) { return kp_landmark.count(kp_idx) > 0; }
    };

    struct Landmark{
        cv::Point3f pt;
        int seen = 0; 
    };

    std::vector<ImagePose> img_pose;
    std::vector<Landmark> landmark;
};

struct dataType {
    cv::Point3d point;
    int red;
    int green;
    int blue; 
};
typedef dataType SpacePoint;
vector<SpacePoint> pointCloud;

void toPly(){
	ofstream outfile("/home/gautham/Documents/Codes/pointcloud.ply");
	outfile << "ply\n" << "format ascii 1.0\n" << "comment VTK generated PLY File\n";
	outfile << "obj_info vtkPolyData points and polygons : vtk4.0\n" << "element vertex " << pointCloud.size() << "\n";
	outfile << "property float x\n" << "property float y\n" << "property float z\n" << "element face 0\n";
	outfile << "property list uchar int vertex_indices\n" << "end_header\n";
	for (int i = 0; i < pointCloud.size(); i++)
	{
		Point3d point = pointCloud.at(i).point;
		outfile << point.x << " ";
		outfile << point.y << " ";
		outfile << point.z << " ";
		outfile << "\n";
	}
	cout<<"PLY SAVE DONE"<<endl;
	outfile.close();
}

class GTSAMBundleAdjust{
    public:
        gtsam::Values output;
        gtsam::Cal3_S2 K_mat;        
        SFM_metadata SFM;

        GTSAMBundleAdjust(SFM_metadata observations){
            SFM = observations;
        }

        void FactorGraphOptimize(){
            using namespace gtsam;
            double cx = SFM.img_pose[0].img.size().width/2;
            double cy = SFM.img_pose[0].img.size().height/2;

            gtsam::Cal3_S2 K(FOCAL_LENGTH, FOCAL_LENGTH,0,cx,cy);

            gtsam::noiseModel::Isotropic::shared_ptr measurement_noise = gtsam::noiseModel::Isotropic::Sigma(2,2.0);
            gtsam::NonlinearFactorGraph graph;
            gtsam::Values initial;

            for(size_t i=0; i<SFM.img_pose.size(); i++){
                auto &img_pose = SFM.img_pose[i];
                gtsam::Rot3 R(
                    img_pose.T.at<double>(0,0),
                    img_pose.T.at<double>(0,1),
                    img_pose.T.at<double>(0,2),

                    img_pose.T.at<double>(1,0),
                    img_pose.T.at<double>(1,1),
                    img_pose.T.at<double>(1,2),

                    img_pose.T.at<double>(2,0),
                    img_pose.T.at<double>(2,1),
                    img_pose.T.at<double>(2,2)
                );
                gtsam::Point3 t;
                t(0) = img_pose.T.at<double>(0,3);
                t(1) = img_pose.T.at<double>(1,3);
                t(2) = img_pose.T.at<double>(2,3);

                gtsam::Pose3 pose(R,t);

                if(i==0){
                    gtsam::noiseModel::Diagonal::shared_ptr pose_noise = gtsam::noiseModel::Diagonal::Sigmas((Vector(6) << Vector3::Constant(0.1), Vector3::Constant(0.1)).finished());
                    graph.emplace_shared<gtsam::PriorFactor<gtsam::Pose3>>(gtsam::Symbol('x',0),pose,pose_noise);
                }
                initial.insert(gtsam::Symbol('x',i),pose);

                for (size_t k=0; k < img_pose.kp.size(); k++) {
                    if (img_pose.kp_3d_exist(k)) {
                        size_t landmark_id = img_pose.kp_3d(k);

                        if (SFM.landmark[landmark_id].seen >= MIN_LANDMARK_SEEN) {
                            gtsam::Point2 pt;

                            pt(0) = img_pose.kp[k].pt.x;
                            pt(1) = img_pose.kp[k].pt.y;

                            graph.emplace_shared<gtsam::GeneralSFMFactor2<gtsam::Cal3_S2>>(pt, measurement_noise, gtsam::Symbol('x', i), gtsam::Symbol('l', landmark_id), gtsam::Symbol('K', 0));
                        }
                    }
                }
            }

            initial.insert(Symbol('K', 0), K);

            noiseModel::Diagonal::shared_ptr cal_noise = noiseModel::Diagonal::Sigmas((Vector(5) << 100, 100, 0.01 /*skew*/, 100, 100).finished());
            graph.emplace_shared<PriorFactor<Cal3_S2>>(Symbol('K', 0), K, cal_noise);

            bool init_prior = false;

            for (size_t i=0; i < SFM.landmark.size(); i++) {
                if (SFM.landmark[i].seen >= MIN_LANDMARK_SEEN) {
                    cv::Point3f &p = SFM.landmark[i].pt;

                    initial.insert<Point3>(Symbol('l', i), Point3(p.x, p.y, p.z));

                    if (!init_prior) {
                        init_prior = true;

                        noiseModel::Isotropic::shared_ptr point_noise = noiseModel::Isotropic::Sigma(3, 0.1);
                        Point3 p(SFM.landmark[i].pt.x, SFM.landmark[i].pt.y, SFM.landmark[i].pt.z);
                        graph.emplace_shared<PriorFactor<Point3>>(Symbol('l', i), p, point_noise);
                    }
                }
            }

            output = LevenbergMarquardtOptimizer(graph, initial).optimize();
            output.print("");

            cout << endl;
            cout << "initial graph error = " << graph.error(initial) << endl;
            cout << "final graph error = " << graph.error(output) << endl;
            K_mat = K;
        }

        void PMVSformatting(){
            using namespace gtsam;
            Matrix3 K_refined = output.at<Cal3_S2>(Symbol('K', 0)).K();

            cout << endl << "final camera matrix K" << endl << K_refined << endl;

            K_refined(0, 0) *= IMAGE_DOWNSAMPLE;
            K_refined(1, 1) *= IMAGE_DOWNSAMPLE;
            K_refined(0, 2) *= IMAGE_DOWNSAMPLE;
            K_refined(1, 2) *= IMAGE_DOWNSAMPLE;

            system("mkdir -p root/visualize");
            system("mkdir -p root/txt");
            system("mkdir -p root/models");

            ofstream option("root/options.txt");

            option << "timages  -1 " << 0 << " " << (SFM.img_pose.size()-1) << endl;;
            option << "oimages 0" << endl;
            option << "level 1" << endl;

            option.close();

            for (size_t i=0; i < SFM.img_pose.size(); i++) {
                Eigen::Matrix<double, 3, 3> R;
                Eigen::Matrix<double, 3, 1> t;
                Eigen::Matrix<double, 3, 4> P;
                char str[256];

                R = output.at<Pose3>(Symbol('x', i)).rotation().matrix();
                t = output.at<Pose3>(Symbol('x', i)).translation().matrix();

                P.block(0, 0, 3, 3) = R.transpose();
                P.col(3) = -R.transpose()*t;
                P = K_refined*P;

                sprintf(str, "cp -f %s/%s root/visualize/%04d.jpg", IMAGE_DIR.c_str(), IMAGES[i].c_str(), (int)i);
                system(str);
                //imwrite(str, SFM.img_pose[i].img);


                sprintf(str, "root/txt/%04d.txt", (int)i);
                ofstream out(str);

                out << "CONTOUR" << endl;

                for (int j=0; j < 3; j++) {
                    for (int k=0; k < 4; k++) {
                        out << P(j, k) << " ";
                    }
                    out << endl;
                }
            }

            cout << endl;
            cout << "Ready to compute PMVS2, options saved at root dir" << endl;
        }
};



class SFMtoolkit{
    public:
        SFM_metadata SFM;
        void feature_proc(){
            using namespace cv;
            using namespace cv::xfeatures2d;

            //Ptr<AKAZE> feature = AKAZE::create();
            //Ptr<SIFT> feature = SIFT::create();
            Ptr<ORB> feature = ORB::create(10000);
            //Ptr<SURF> feature = SURF::create();
            Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

            namedWindow("img", WINDOW_NORMAL);
            cout<<"CKPT 1 "<<endl;
            cv::glob(img_dir2, impath, false);

            for (auto f : impath) {
                cout<<"Image ::: "<<f<<endl;
                IMAGES.push_back(f);
                SFM_metadata::ImagePose a;

                Mat img = imread(f);
                assert(!img.empty());

                resize(img, img, img.size()/IMAGE_DOWNSAMPLE);
                a.img = img;
                cvtColor(img, img, COLOR_BGR2GRAY);

                feature->detect(img, a.kp);
                feature->compute(img, a.kp, a.desc);

                SFM.img_pose.emplace_back(a);
            }
        }

        void FeatureMatch(){
            using namespace cv;
            using namespace xfeatures2d;
            Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
            cout<<"CKPT 2 "<<endl;
            int count = 0;

            for (size_t i=0; i < SFM.img_pose.size()-1; i++) {
                auto &img_pose_i = SFM.img_pose[i];
                for (size_t j=i+1; j < SFM.img_pose.size(); j++) {
                    auto &img_pose_j = SFM.img_pose[j];
                    vector<vector<DMatch>> matches;
                    vector<Point2f> src, dst;
                    vector<uchar> mask;
                    vector<int> i_kp, j_kp;

                    matcher->knnMatch(img_pose_i.desc, img_pose_j.desc, matches, 2);

                    for (auto &m : matches) {
                        if(m[0].distance < 0.7*m[1].distance) {
                            src.push_back(img_pose_i.kp[m[0].queryIdx].pt);
                            dst.push_back(img_pose_j.kp[m[0].trainIdx].pt);

                            i_kp.push_back(m[0].queryIdx);
                            j_kp.push_back(m[0].trainIdx);
                        }
                    }


                    findFundamentalMat(src, dst, FM_RANSAC, 3.0, 0.99, mask);

                    Mat canvas = img_pose_i.img.clone();
                    Mat buffer_canvas = img_pose_j.img.clone();
                    cv::drawKeypoints(canvas, img_pose_i.kp, canvas,Scalar(255,0,0));
                    cv::drawKeypoints(buffer_canvas, img_pose_j.kp, buffer_canvas,Scalar(0,0,255));
                    canvas.push_back(buffer_canvas);

                    for (size_t k=0; k < mask.size(); k++) {
                        if (mask[k]) {
                            img_pose_i.kp_match_idx(i_kp[k], j) = j_kp[k];
                            img_pose_j.kp_match_idx(j_kp[k], i) = i_kp[k];
                            line(canvas, src[k], dst[k] + Point2f(0, img_pose_i.img.rows), Scalar(0, 255, 0), 1);
                        }
                    }

                    int good_matches = sum(mask)[0];
                    assert(good_matches >= 10);

                    cout << "Feature matching " << i << " " << j << " ==> " << good_matches << "/" << matches.size() << endl;

                    resize(canvas, canvas, canvas.size()/2);
                    if(count>0){
                        cv::namedWindow("img");
                        imshow("img", canvas);
                        waitKey(1000);
                    }
                    count++;
                }
                    cv::destroyAllWindows();
            }
        }

        void sfm_reconstruct(){
            using namespace cv;
            using namespace viz;

            double cx = SFM.img_pose[0].img.size().width/2;
            double cy = SFM.img_pose[0].img.size().height/2;

            Point2d pp(cx, cy);

            Mat K = Mat::eye(3, 3, CV_64F);

            K.at<double>(0,0) = FOCAL_LENGTH;
            K.at<double>(1,1) = FOCAL_LENGTH;
            K.at<double>(0,2) = cx;
            K.at<double>(1,2) = cy;

            cout << endl << "initial camera matrix K " << endl << K << endl << endl;

            SFM.img_pose[0].T = Mat::eye(4, 4, CV_64F);
            SFM.img_pose[0].P = K*Mat::eye(3, 4, CV_64F);

            for (size_t i=0; i < SFM.img_pose.size() - 1; i++) {
                auto &prev = SFM.img_pose[i];
                auto &cur = SFM.img_pose[i+1];

                vector<Point2f> src, dst;
                vector<size_t> kp_used;

                for (size_t k=0; k < prev.kp.size(); k++) {
                    if (prev.kp_match_exist(k, i+1)) {
                        size_t match_idx = prev.kp_match_idx(k, i+1);

                        src.push_back(prev.kp[k].pt);
                        dst.push_back(cur.kp[match_idx].pt);

                        kp_used.push_back(k);
                    }
                }

                Mat mask;

                Mat E = findEssentialMat(dst, src, FOCAL_LENGTH, pp, RANSAC, 0.999, 1.0, mask);
                Mat local_R, local_t;

                //cout<<E<<endl;

                recoverPose(E, dst, src, local_R, local_t, FOCAL_LENGTH, pp, mask);

                Mat T = Mat::eye(4, 4, CV_64F);
                local_R.copyTo(T(Range(0, 3), Range(0, 3)));
                local_t.copyTo(T(Range(0, 3), Range(3, 4)));

                cur.T = prev.T*T;

                Mat R = cur.T(Range(0, 3), Range(0, 3));
                Mat t = cur.T(Range(0, 3), Range(3, 4));

                Mat P(3, 4, CV_64F);

                P(Range(0, 3), Range(0, 3)) = R.t();
                P(Range(0, 3), Range(3, 4)) = -R.t()*t;
                P = K*P;

                cur.P = P;

                Mat points4D;
                triangulatePoints(prev.P, cur.P, src, dst, points4D);

                if (i > 0) {
                    double scale = 0;
                    int count = 0;

                    Point3f prev_camera;

                    prev_camera.x = prev.T.at<double>(0, 3);
                    prev_camera.y = prev.T.at<double>(1, 3);
                    prev_camera.z = prev.T.at<double>(2, 3);

                    vector<Point3f> new_pts;
                    vector<Point3f> existing_pts;
                    cout<<"KP USED size : "<<kp_used.size()<<endl;
                    for (size_t j=0; j < kp_used.size(); j++) {
                        size_t k = kp_used[j];
                        //cout<<"  Mask.at j : "<<mask.at<uchar>(j)<<" prev.kp_ex : "<<prev.kp_match_exist(k, i+1)<<" prev.kp3d_ex : "<<prev.kp_3d_exist(k)<<endl;
                        if (mask.at<uchar>(j) && prev.kp_match_exist(k, i+1) && prev.kp_3d_exist(k)) {
                            Point3f pt3d;

                            pt3d.x = points4D.at<float>(0, j) / points4D.at<float>(3, j);
                            pt3d.y = points4D.at<float>(1, j) / points4D.at<float>(3, j);
                            pt3d.z = points4D.at<float>(2, j) / points4D.at<float>(3, j);

                            size_t idx = prev.kp_3d(k);
                            Point3f avg_landmark = SFM.landmark[idx].pt / (SFM.landmark[idx].seen - 1);
                            count++;
                            new_pts.push_back(pt3d);
                            existing_pts.push_back(avg_landmark);
                        }
                    }
                    cout<<"NEW point size is as : "<<new_pts.size()<<endl;
                    if(new_pts.size()<=0){
                        cout<<"SKIPPING FRAME"<<endl;
                        continue;
                    } 
                    for (size_t j=0; j < new_pts.size()-1; j++) {
                        for (size_t k=j+1; k< new_pts.size(); k++) {
                            double s = norm(existing_pts[j] - existing_pts[k]) / norm(new_pts[j] - new_pts[k]);

                            scale += s;
                            count++;
                        }
                    }

                    assert(count > 0);

                    scale /= count;

                    cout << "image " << (i+1) << " ==> " << i << " scale=" << scale << " count=" << count <<  endl;

                    local_t *= scale;

                    Mat T = Mat::eye(4, 4, CV_64F);
                    local_R.copyTo(T(Range(0, 3), Range(0, 3)));
                    local_t.copyTo(T(Range(0, 3), Range(3, 4)));

                    cur.T = prev.T*T;

                    R = cur.T(Range(0, 3), Range(0, 3));
                    t = cur.T(Range(0, 3), Range(3, 4));

                    Mat P(3, 4, CV_64F);
                    P(Range(0, 3), Range(0, 3)) = R.t();
                    P(Range(0, 3), Range(3, 4)) = -R.t()*t;
                    P = K*P;

                    cur.P = P;

                    triangulatePoints(prev.P, cur.P, src, dst, points4D);
                }

                for (size_t j=0; j < kp_used.size(); j++) {
                    if (mask.at<uchar>(j)) {
                        size_t k = kp_used[j];
                        size_t match_idx = prev.kp_match_idx(k, i+1);

                        Point3f pt3d;

                        pt3d.x = points4D.at<float>(0, j) / points4D.at<float>(3, j);
                        pt3d.y = points4D.at<float>(1, j) / points4D.at<float>(3, j);
                        pt3d.z = points4D.at<float>(2, j) / points4D.at<float>(3, j);

                        if (prev.kp_3d_exist(k)) {
                            cur.kp_3d(match_idx) = prev.kp_3d(k);

                            SFM.landmark[prev.kp_3d(k)].pt += pt3d;
                            SFM.landmark[cur.kp_3d(match_idx)].seen++;
                        } else {
                            SFM_metadata::Landmark landmark;

                            landmark.pt = pt3d;
                            landmark.seen = 2;

                            SFM.landmark.push_back(landmark);

                            prev.kp_3d(k) = SFM.landmark.size() - 1;
                            cur.kp_3d(match_idx) = SFM.landmark.size() - 1;
                        }
                    }
                }
            }
            vector<Mat> pts_est;
            for (auto &l : SFM.landmark) {
                if (l.seen >= 3) {
                    SpacePoint pts;
                    l.pt /= (l.seen - 1);
                    pts.point.x = l.pt.x;
                    pts.point.y = l.pt.y;
                    pts.point.z = l.pt.z;
                    pts.red = 1; pts.blue = 1; pts.green = 1;
                    pointCloud.push_back(pts);
                }
            }
            toPly();
        }
};


int main(int argc, char **argv){
    SFMtoolkit stk;
    stk.feature_proc();
    stk.FeatureMatch();
    stk.sfm_reconstruct();

    GTSAMBundleAdjust gtb(stk.SFM);
    gtb.FactorGraphOptimize();
    gtb.PMVSformatting();
    cout<<"EXECUTED"<<endl;
	return 0;
}
