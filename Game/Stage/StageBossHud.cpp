// TH06-style presentation adapted from th06/src/Gui.cpp UpdateStageElements.
// Game state remains owned by the native TH05 boss implementations.
#include "Stage.h"
#include "../Game.h"
#include "../Practice/Practice.h"
#include "Boss/Boss.h"
#include "Boss/BossStage4.h"
#include "Boss/BossStage4Mai.h"
#include "Boss/BossStage4Yuki.h"
#include "MidBoss/MidBoss.h"
#include "Dialogue/Dialogue.h"
#include "../../gameLib/Graphic/Th5ExtFont.h"
#include <cstdio>

namespace th5w {
namespace {
struct HealthPhase {int phase,start,end,enrage,cut2;};
void Number(int x,int y,int value,float r,float g,float b,float alpha,bool right=false){
 char text[16];sprintf_s(text,"%d",value<0?0:value);
 int length=0;while(text[length])++length;
 if(right)x-=length*16;
 for(int i=0;i<length;i++)CTh5ExtFont::DrawExtChar(0xa0+text[i]-'0',x+i*16,y,r,g,b,alpha);
}
}

practice::BossBarData CStage::GetBossBarData() const {
 practice::BossBarData out;
 if(m_bLogo||m_bQuit||m_bContinueScr||m_pDialogue->IsRunning())return out;
 const HealthPhase* phases=NULL;int count=0,phase=0,hp=0,variant=0,extra=0;
 const auto use=[&](const HealthPhase* list,int size){phases=list;count=size;};
 const int stage=CGame::GVar().m_playStage;
 if(m_pBoss){
  if(m_pBoss->m_curPhase>=0xfd||m_pBoss->m_lastDrawnHP<=0)return out;
  phase=m_pBoss->m_curPhase;hp=m_pBoss->m_curHP;variant=1;
  static const HealthPhase s1[]={{2,4650,2550,0,0},{3,2550,450,0,0},{4,450,0,0,0}};
  static const HealthPhase s2[]={{2,4400,3000,0,0},{4,3000,1900,0,0},{5,1900,500,0,0},{7,500,0,0,0}};
  static const HealthPhase s3[]={{3,9600,7400,0,0},{6,7400,5200,0,0},{9,5200,3000,0,0},{12,3000,800,0,0},{14,800,0,0,0}};
  static const HealthPhase both[]={{2,9000,0,5500,2250}};
  static const HealthPhase mai[]={{3,7800,5800,0,0},{5,5800,2800,0,0},{7,2800,1200,0,0},{9,1200,0,0,0}};
  static const HealthPhase yuki[]={{3,7900,4600,0,0},{5,4600,3200,0,0},{7,3200,1200,0,0},{9,1200,0,0,0}};
  static const HealthPhase s5[]={{2,8300,7500,0,0},{4,7500,5700,6300,0},{5,5700,4500,0,0},{7,4500,2700,3200,0},{8,2700,1200,0,0},{10,1200,0,0,0}};
  static const HealthPhase s6[]={{2,22800,20600,0,0},{3,20600,18400,0,0},{6,18400,14600,0,0},{7,14600,11600,0,0},{9,11600,8600,0,0},{10,8600,2800,5600,3800},{12,2800,0,0,0}};
  static const HealthPhase ex[]={{2,26500,23800,0,0},{4,23800,21000,0,0},{6,21000,18100,0,0},{8,18100,15100,0,0},{10,15100,12600,0,0},{12,12600,9600,0,0},{14,9600,6800,0,0},{16,6800,3400,0,0},{17,3400,0,0,0}};
  switch(stage){
   case 0:use(s1,3);break;
   case 1:use(s2,4);break;
   case 2:use(s3,5);break;
   case 3:
    if(dynamic_cast<CBossStage4Mai*>(m_pBoss)){use(mai,4);variant=2;}
    else if(dynamic_cast<CBossStage4Yuki*>(m_pBoss)){use(yuki,4);variant=3;}
    else {use(both,1);hp+=static_cast<CBossStage4*>(m_pBoss)->m_2curHP;extra=4;}
    break;
   case 4:use(s5,6);break;
   case 5:use(s6,7);break;
   case 6:use(ex,9);break;
  }
 }else if(m_pMidBoss){
  if(m_pMidBoss->m_curPhase>=0xfe||m_pMidBoss->m_lastDrawnHP<=0)return out;
  phase=m_pMidBoss->m_curPhase;hp=m_pMidBoss->m_curHP;variant=4;
  static const HealthPhase m1[]={{3,1000,0,200,0}},m2[]={{1,1400,400,800,600},{2,400,0,0,0}};
  static const HealthPhase m3[]={{1,1400,0,0,0}},m4[]={{1,1100,0,0,0}},m5[]={{1,1550,0,0,0}},mx[]={{1,3000,0,1000,0}};
  switch(stage){case 0:use(m1,1);break;case 1:use(m2,2);break;case 2:use(m3,1);break;case 3:use(m4,1);break;case 4:use(m5,1);break;case 6:use(mx,1);break;}
 }
 for(int i=0;i<count;i++)if(phase<=phases[i].phase){
  out.present=true;out.key=stage*1000+variant*100+i;out.hp=hp;
  out.start=phases[i].start;out.end=phases[i].end;out.enrage=phases[i].enrage;out.cut2=phases[i].cut2;
  out.remaining=count-i-1+extra;
  // The original stage-2 midboss keeps its HP on timeout, rather than clamping to 400.
  if(variant==4&&stage==1&&i==1&&hp>out.start)out.start=hp;
  break;
 }
 return out;
}

void CStage::TickBossBar(){
 if(!practice::Options().eosdBar){m_bossBar=practice::BossBarAnimation();return;}
 m_bossBar.Step(GetBossBarData());
}

void CStage::DrawBossBar(){
 if(!m_bossBar.opacity||m_bLogo||m_bQuit||m_pDialogue->IsRunning())return;
 const float alpha=m_bossBar.opacity/255.f;
 const auto& data=m_bossBar.data;
 const int x=m_playAreaUpperLeftX,y=m_playAreaUpperLeftY;
 const unsigned char enemy[]={0xea,0xeb,0xec,0};
 CTh5ExtFont::DrawExtString(enemy,3,x,y,1,1,0,alpha);
 Number(x+48,y,data.remaining,1,1,.5f,alpha);
 const int left=x+64,width=272,filled=int(m_bossBar.displayed*width);
 if(filled>0){
  CCommonFunctionGraphic::DrawRectangle(float(left),float(y+8),float(left+filled-1),float(y+9),1,1,0,alpha);
  const int thresholds[]={data.enrage,data.cut2};
  if(data.enrage>data.end){
   int red=(data.enrage-data.end)*width/(data.start-data.end);if(red>filled)red=filled;
   if(red>0)CCommonFunctionGraphic::DrawRectangle(float(left),float(y+8),float(left+red-1),float(y+9),1,.25f,.25f,alpha);
  }
  for(int i=0;i<2;i++)if(thresholds[i]>data.end&&thresholds[i]<data.start){
   const int cut=(thresholds[i]-data.end)*width/(data.start-data.end);
   if(cut>0&&cut<filled)CCommonFunctionGraphic::DrawRectangle(float(left+cut),float(y+8),float(left+cut),float(y+9),0,0,0,alpha);
  }
 }
 const practice::PhaseTimeout timer=GetPhaseTimeout();
 if(timer.unit!=practice::PhaseTimeout::None){
  const bool turns=timer.unit==practice::PhaseTimeout::Turns;
  const float tint=timer.value<=(turns?5:10)?.65f:1.f;
  const int right=x+384;
  Number(right-(turns?16:0),y,timer.value,1,tint,tint,alpha,true);
  if(turns)CTh5ExtFont::DrawExtChar(0xbd,right-16,y,1,tint,tint,alpha);
 }
}
}
