#pragma once
namespace th5w { namespace practice {
struct BossBarData {
 bool present;int key,hp,start,end,remaining,enrage,cut2;
 BossBarData():present(false),key(0),hp(0),start(1),end(0),remaining(0),enrage(0),cut2(0){}
};
// TH06 Gui::UpdateStageElements: opacity +/-4, health fill +.01 / depletion -.02.
struct BossBarAnimation {
 float displayed;int opacity,key,rangeStart;BossBarData data;
 BossBarAnimation():displayed(0),opacity(0),key(-1),rangeStart(1){}
 void Step(BossBarData input){
  if(input.present){
   if(key!=input.key){key=input.key;rangeStart=input.start;}
   input.start=rangeStart;data=input;
   opacity=opacity<252?opacity+4:255;
   float target=float(input.hp-input.end)/float(input.start-input.end);
   if(target<0)target=0;if(target>1)target=1;
   if(displayed<target){displayed+=.01f;if(displayed>target)displayed=target;}
   else if(displayed>target){displayed-=.02f;if(displayed<target)displayed=target;}
  }else{
   opacity=opacity>4?opacity-4:0;
   if(!opacity){displayed=0;key=-1;data=BossBarData();}
  }
 }
};
} }
