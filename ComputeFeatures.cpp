//
// Created by GING on 2018-12-17.
//

#include "ComputeFeatures.h"


void Compute_SIFT_Feature_Single(const std::string & in_sImgPath,std::vector<cv::KeyPoint> & out_KeyPoints,MAT_DESC & out_Descs)
{
    cv::Mat image=cv::imread(in_sImgPath);
    cv::Ptr<cv::Feature2D> sift_detector = cv::xfeatures2d::SIFT::create();

    sift_detector->detectAndCompute(image,cv::noArray(),out_KeyPoints,out_Descs);

}


void Compute_SIFT_Features_All(const MAP_IMGS & in_images,
        MAP_IMGS & out_valid_images,
        MAP_KEYPOINTS & out_all_keypoints ,
        MAP_DESCS & out_all_descs)
{
    std::vector<cv::KeyPoint> kps;
    MAT_DESC descs;
    for(const auto & iter:in_images)
    {
        uint32_t dImgID=iter.first;
        const std::string sImgName=iter.second;

        Compute_SIFT_Feature_Single(sImgName,kps,descs);

        if(kps.size()>10)
        {
            out_valid_images[dImgID]=sImgName;

            MAP_KEYPOINT map_kps;
            for(MAP_KEYPOINTS ::size_type i=0;i<kps.size();++i)
            {
                map_kps[i]=kps[i];
            }
            out_all_keypoints[dImgID]=map_kps;
            out_all_descs[dImgID]=descs;
        }
    }

}

void Compute_SIFT_Feature_Single_Color(const std::string & in_sImgPath,std::vector<cv::KeyPoint> & out_KeyPoints,MAT_DESC & out_Descs,
                                       MAP_COLOR & out_color)
{
    cv::Mat image=cv::imread(in_sImgPath);
    cv::Ptr<cv::Feature2D> sift_detector = cv::xfeatures2d::SIFT::create();


    sift_detector->detectAndCompute(image,cv::noArray(),out_KeyPoints,out_Descs);

    std::vector<cv::KeyPoint>::size_type i{0};
    for(const auto & iter:out_KeyPoints)
    {
        const cv::Point2f & p=iter.pt;
        cv::Vec3b color=image.at<cv::Vec3b>(p.y,p.x);
        out_color[i]=color;
        ++i;
    }
}

void Compute_SIFT_Features_All_Colors(const MAP_IMGS & in_images,
                                      MAP_IMGS & out_valid_images,
                                      MAP_KEYPOINTS & out_all_keypoints ,
                                      MAP_DESCS & out_all_descs,
                                      MAP_COLORS & out_all_colors)
{
    std::vector<cv::KeyPoint> kps;
    MAT_DESC descs;
    for(const auto & iter:in_images)
    {
        uint32_t dImgID=iter.first;
        const std::string sImgName=iter.second;

        MAP_COLOR map_color;

        Compute_SIFT_Feature_Single_Color(sImgName,kps,descs,map_color);

        if(kps.size()>10)
        {
            out_valid_images[dImgID]=sImgName;

            MAP_KEYPOINT map_kps;
            for(MAP_KEYPOINTS::size_type i=0;i<kps.size();++i)
            {
                map_kps[i]=kps[i];
            }
            out_all_keypoints[dImgID]=map_kps;
            out_all_descs[dImgID]=descs;
            out_all_colors[dImgID]=map_color;
        }
    }
}


void Convert_MapKps_to_VecKps(const MAP_KEYPOINT & in_MapKps,
        std::vector<cv::KeyPoint> out_kps)
{
    for(const auto & iter:in_MapKps)
    {
        out_kps.push_back(iter.second);
    }
}



