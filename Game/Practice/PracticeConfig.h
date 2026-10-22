#pragma once
namespace th5w { namespace practice {
// Versioned replay payload: fixed-width x86 integers, never raw bool/padding.
struct Config {
 int mode,stage,warp,frame,lives,bombs,score,power,graze,points,totalPoints,dream,rank,lockRank;
 Config():mode(0),stage(0),warp(0),frame(0),lives(8),bombs(8),score(0),power(128),graze(0),points(0),totalPoints(0),dream(128),rank(32),lockRank(0){}
 static int Bound(int n,int lo,int hi){return n<lo?lo:n>hi?hi:n;}
 static int RankMin(int difficulty){const int v[]={16,24,44,48,32};return v[Bound(difficulty,0,4)];}
 static int RankMax(int difficulty){const int v[]={32,40,54,58,32};return v[Bound(difficulty,0,4)];}
 void Clamp(int difficulty){
  mode=Bound(mode,0,1);stage=Bound(stage,0,6);warp=Bound(warp,0,3);frame=Bound(frame,0,60000);
  lives=Bound(lives,0,8);bombs=Bound(bombs,0,8);score=Bound(score,0,99999990)/10*10;
  power=Bound(power,1,128);graze=Bound(graze,0,999);points=Bound(points,0,999);totalPoints=Bound(totalPoints,0,99999);dream=Bound(dream,1,128);
  lockRank=lockRank?1:0;rank=Bound(rank,lockRank?0:RankMin(difficulty),lockRank?64:RankMax(difficulty));
 }
 bool Valid(int difficulty) const {
  Config copy=*this;copy.Clamp(difficulty);
  return mode==copy.mode&&stage==copy.stage&&warp==copy.warp&&frame==copy.frame&&lives==copy.lives&&bombs==copy.bombs&&score==copy.score&&power==copy.power&&graze==copy.graze&&points==copy.points&&totalPoints==copy.totalPoints&&dream==copy.dream&&rank==copy.rank&&lockRank==copy.lockRank;
 }
};
static_assert(sizeof(Config)==14*4,"Practice replay field size");
} }
