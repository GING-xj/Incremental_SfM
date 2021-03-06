//
// Created by GING on 2018-12-17.
//

#include "ComputeTracks.h"

void Compute_Tracks(UnionFind &uf_tree, MAP_MATCHES & in_MapMatches,flat_pair_map<std::pair<int,int>,int> & out_map_node_to_index)
{
    //ImgID,FeatId
    //as a node to compute track
    using NODE=std::pair<int,int>;
    std::set< NODE > all_feats_to_node;

    for(const auto &  match:in_MapMatches)
    {
        int I=match.first.first;
        int J=match.first.second;

        const VEC_MATCHES & vec_matches=match.second;
        for(const auto feat: vec_matches)
        {
            all_feats_to_node.emplace(I,feat.first);
            all_feats_to_node.emplace(J,feat.second);
        }
    }

    //give every node an index
    int count{0};
    for(const auto & feat_node:all_feats_to_node)
    {
        out_map_node_to_index.emplace_back(feat_node,count);
        ++count;
    }

    //use NODE to sort
    //make a track a sorted track
    out_map_node_to_index.sort() ;

    //Initialize uf_tree
    uf_tree.InitSets(out_map_node_to_index.size());

    //Union Tracks
    for(const auto & match:in_MapMatches)
    {
        //link all features in I and J as a track
        int I=match.first.first;
        int J=match.first.second;

        for(const auto & feat:match.second)
        {
            NODE node1(I,feat.first);
            NODE node2(J,feat.second);

            //union there node index
            uf_tree.Union(out_map_node_to_index[node1],out_map_node_to_index[node2]);
        }
    }
}

//Remove tracks with more than one feature in one image
//Remove tracks if too short
void Filter_Tracks(UnionFind & in_ufTree,flat_pair_map<std::pair<int,int>,int> & in_map_node_to_index,MAP_TRACKS & out_tracks)
{
    //{trackID, {ImageID}} as a track
    std::map<int, std::set<int>> tracks;

    //tracks ID that has problem
    std::set<int> problem_tracks;


    //filter tracks with more than one feature in the same image
    for(int i=0;i<in_map_node_to_index.size();++i)
    {
        //find i-th node's track ID
        const int & d_TrackID=in_ufTree.Find(i);

        //if problem track has been marked
        if(problem_tracks.count(d_TrackID)!=0)
        {
            continue;
        }

        //i-th element of map_node_to_index
        const auto & elem=in_map_node_to_index[i];
        //if imageID has been set in tracks
        if(tracks[d_TrackID].count(elem.first.first))
        {
            problem_tracks.insert(d_TrackID);
        }
        else
        {
            tracks[d_TrackID].insert(elem.first.first);
        }
    }

    //filter tracks that's too short
    for(const auto & track:tracks)
    {
        if(track.second.size()<2)
        {
            //insert track ID
            problem_tracks.insert(track.first);
        }
    }

    //set problem track in uf-tree
    for(unsigned int & root_index:in_ufTree.m_cc_parent)
    {
        if(problem_tracks.count(root_index)!=0)
        {
            in_ufTree.m_cc_size[root_index]=1;
            root_index=std::numeric_limits<unsigned >::max();
        }
    }

    //Export Good Track to out_tracks
    for(uint32_t i=0;i<in_map_node_to_index.size();++i)
    {
        const auto & elem=in_map_node_to_index[i];
        int trackID=in_ufTree.m_cc_parent[i];

        if(trackID!=std::numeric_limits<unsigned>::max() && in_ufTree.m_cc_size[trackID]>1 )
        {
            out_tracks[trackID].insert(elem.first);
        }
    }

}

