#include "../Game/Game.h"
#include "../Game/Original/OriginalScenes.h"
#include "../Game/TitleScreen/TitleScreen.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/EnemyBullet/EnemyBullet.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Item/Item.h"
#include "../Game/Stage/Boss/BossStage1.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
namespace th5w {
struct PracticeTests {
 static int checks;
 static void Check(bool ok,const char* reason){++checks;if(!ok){fprintf(stderr,"FAIL: %s\n",reason);exit(1);}}
 static void Screenshot(const char* name){std::vector<unsigned char> b(640*480*3);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,640,480,GL_BGR_EXT,GL_UNSIGNED_BYTE,b.data());BITMAPFILEHEADER f={};f.bfType=0x4d42;f.bfOffBits=sizeof(f)+sizeof(BITMAPINFOHEADER);f.bfSize=f.bfOffBits+DWORD(b.size());BITMAPINFOHEADER h={};h.biSize=sizeof(h);h.biWidth=640;h.biHeight=480;h.biPlanes=1;h.biBitCount=24;FILE* out=fopen(name,"wb");fwrite(&f,sizeof(f),1,out);fwrite(&h,sizeof(h),1,out);fwrite(b.data(),b.size(),1,out);fclose(out);}
 static original::RunStats Perfect(){original::RunStats s;s.frames=1000;s.stage=5;s.complete=true;s.endSequence=3;s.stdFrames=46000;s.enemiesGone=s.enemiesKilled=100;s.itemsSpawned=s.itemsCollected=s.points=s.maxPoints=100;return s;}
 static void VerdictMath(){using namespace original;RunStats s=Perfect();auto v=Evaluate(s);Check(v.valid&&v.skill==612500&&v.maximum==1000000&&v.row==12,"original Normal base skill, cap and remark row");for(int i=0;i<4;i++)Check(v.fractions[i]==1000000,"equal numerator and denominator bypass integer rounding");Check(v.fractions[4]==350000,"original spirit configuration bonus");s.stage=6;s.endSequence=1;Check(Evaluate(s).skill==1168750,"original Extra skill adjustment");s=Perfect();s.difficulty=3;Check(Evaluate(s).skill==1218750,"original Lunatic skill adjustment");s=Perfect();s.highestScore=100000000;Check(Evaluate(s).skill==1000000,"100 million score bonus respects difficulty cap");s=Perfect();s.complete=false;s.endSequence=0;s.stdFrames=23000;v=Evaluate(s);Check(v.fractions[0]==483000&&v.skill==385145,"original divide-before-multiply percentage and failed-run penalty");s=Perfect();s.slowFrames=500;Check(!Evaluate(s).valid,"half slow frames invalidates verdict");s.slowFrames=499;Check(Evaluate(s).valid,"one frame below invalidation remains eligible");s=Perfect();s.misses=10;s.bombs=15;Check(Evaluate(s).skill==79166,"original miss and bomb penalties with spirit");s=Perfect();s.finalScore=80000000;s.bombs=20;Check(Evaluate(s).comment==0,"unknown resident byte is not bombs_used");s.legacyUnknown=15;Check(Evaluate(s).comment==8,"preserved original unknown-byte comment condition");s=Perfect();s.endSequence=0;Check(Evaluate(s).comment==5,"original failure comment compares bombs <= twice misses");s.bombs=1;Check(Evaluate(s).comment==6,"late failure with few point items");s=Perfect();s.itemsSpawned=3;s.itemsCollected=2;Check(Evaluate(s).fractions[2]==666666,"percentage retains original truncation");s=RunStats();s.frames=100;v=Evaluate(s);Check(v.fractions[1]==0&&v.fractions[2]==0,"zero denominators do not fabricate percentages");}
 static void Scenes(CGame& game){using namespace original;Stats()=Perfect();for(int i=0;i<6;i++)Stats().stageScores[i]=(i+1)*1234560;Stats().finalScore=12345670;
  {COriginalEnd scene(COriginalEnd::VerdictOnly);Check(scene.Initialize(),"verdict PI and remarks load from retail DAT");Check(scene.m_comments[0].size()>0&&scene.m_comments[34].size()>0,"all original fixed-width remark records load");scene.m_age=128;scene.Draw();glFinish();Screenshot("original-verdict.bmp");scene.Tick(32);Check(!scene.m_fadeOut,"verdict waits for release before accepting input");scene.Tick(0);scene.Tick(32);Check(scene.m_fadeOut==1,"verdict dismisses on a fresh native input");}
  {COriginalEnd scene(COriginalEnd::Staff);Check(scene.Initialize(),"all eleven Staff CDGs and twelve original sprites load");Check(scene.m_cards.NImage()==11&&scene.m_sprites.NImage()==12,"Staff assets have expected original layout");Check(scene.m_palette[39]>0&&scene.m_palette[40]>0&&scene.m_palette[41]>0,"Staff BFT palette supplies visible verdict and credits");scene.m_testMeasure=0;for(int i=0;i<660;i++)scene.Tick(32);Check(scene.m_phase==0,"Staff waits for music measure and cannot be skipped early");scene.m_testMeasure=30;scene.Tick(0);Check(scene.m_phase==1&&scene.m_orb.active,"measure 30 starts original orb gathering");for(int i=0;i<32;i++)scene.Tick(0);Check(scene.m_phase==2&&scene.m_orbRadius==16,"gather lasts thirty-two frames");scene.m_testMeasure=48;for(int i=0;i<512;i++)scene.Tick(0);Check(scene.m_phase==3,"camera opening and measure 48 precede first credit");for(int i=0;i<300;i++)scene.Tick(0);scene.Draw();glFinish();Screenshot("original-staff-first.bmp");
   const int expected[]={0,2,3,5,6,7,8,9,10};int reached=0;for(int i=0;i<15000&&scene.m_phase!=6;i++){scene.m_testMeasure=300;if(scene.m_card==expected[reached]&&reached<8)++reached;scene.Tick(0);}Check(scene.m_phase==6&&scene.m_card==10&&reached==8,"original credit order reaches final card");for(int i=0;i<400;i++)scene.Tick(0);scene.Draw();glFinish();Screenshot("original-staff-verdict.bmp");scene.Tick(32);for(int i=0;i<44;i++)scene.Tick(0);Check(scene.m_scroll==352&&!scene.m_fadeOut,"Z slides to native six-stage score page");scene.Draw();glFinish();Screenshot("original-staff-scores.bmp");scene.Tick(1);for(int i=0;i<44;i++)scene.Tick(0);Check(!scene.m_scroll,"Up returns to verdict page");scene.Tick(16);Check(scene.m_fadeOut==1,"X exits Staff through its fade");}
  for(int chara=0;chara<4;chara++){game.m_globalVar.m_playChara=chara;COriginalEnd scene(COriginalEnd::AllCast);Check(scene.Initialize(),"All Cast first quarter loads");for(int screen=0;screen<7;screen++){scene.m_castScreen=screen;scene.m_castLine=0;scene.LoadCastPicture();Check(scene.m_background&&scene.m_background->m_width==640&&scene.m_background->m_height==400,"all original character-dependent cast PI quarters decode");}scene.m_castScreen=0;scene.LoadCastPicture();scene.m_testMeasure=0;for(int i=0;i<88;i++)scene.Tick(0);Check(scene.m_castState==0,"All Cast waits for measure 2");scene.m_testMeasure=2;for(int i=0;i<86;i++)scene.Tick(0);scene.m_testMeasure=4;for(int i=0;i<22;i++)scene.Tick(0);Check(scene.m_castLine==1,"first cast line follows masked reveal");scene.Draw();glFinish();if(chara==0)Screenshot("original-allcast.bmp");scene.m_testMeasure=10000;for(int i=0;i<5000&&!scene.m_fadeOut;i++)scene.Tick(0);Check(scene.m_castScreen==6&&scene.m_fadeOut==1,"all seven cast screens finish without loading null eighth filename");}
 }
 static void Routes(CGame& game){using namespace original;CScheme* next=NULL;int id=0;auto& g=game.m_globalVar;g.m_bPracticeMode=g.m_bReplayMode=false;Stats()=Perfect();g.m_playStage=5;Check(CSchemeSwitcher::SwitchScheme(&next,&id,5,1)&&id==9,"normal ending routes to Staff before registration");delete next;Check(CSchemeSwitcher::SwitchScheme(&next,&id,9,1)&&id==7,"Staff verdict and scores finish at highscore registration");delete next;g.m_playStage=6;Stats().stage=6;Stats().endSequence=1;Check(CSchemeSwitcher::SwitchScheme(&next,&id,5,1)&&id==10,"Extra ending routes to All Cast");delete next;Check(CSchemeSwitcher::SwitchScheme(&next,&id,10,1)&&id==11,"Extra All Cast precedes standalone verdict");delete next;Check(CSchemeSwitcher::SwitchScheme(&next,&id,11,1)&&id==7,"Extra verdict precedes registration");delete next;Stats().complete=false;Check(CSchemeSwitcher::SwitchScheme(&next,&id,7,3)&&id==11,"failed run registration precedes verdict");delete next;Check(CSchemeSwitcher::SwitchScheme(&next,&id,11,1)&&id==6,"failed run verdict retains port replay-save flow");delete next;}
 static void DemoData(CGame& game){using namespace original;auto& g=game.m_globalVar;StopDemo(true);bool extras[4];memcpy(extras,g.m_bExtraPlayable,4);memset(g.m_bExtraPlayable,0,4);Check(!StartDemo(true)&&!DemoActive(),"hidden Extra requires all four original unlock flags");
  for(int number=1;number<=4;number++){Check(StartDemo()&&Demo().number==number,"original normal Demo sequence rotates through four characters");uint32_t seed=318;Demo().seed=318;for(int i=0;i<16;i++){seed=seed*0x015a4e35u+1;Check(IRandom()==int((seed>>16)&32767),"Demo uses original IRand LCG");}const auto bytes=Demo().data;unsigned short input=0;for(int i=0;i<4996;i++){Check(DemoInput(15|64|128,input),"movement, slow and skip do not cancel Demo");Check(input==(bytes[i]|(bytes[5000+i]?64:0)),"original input and shift planes decode byte-for-byte");}Check(!DemoInput(0,input),"normal Demo ends before original 5000-byte buffer limit");Check(!g.m_bCanSaveReplay,"Demo cannot save replay");const auto oldClear=g.m_bClear[g.m_playChara][g.m_playDifficulty];g.OnClear();Check(g.m_bClear[g.m_playChara][g.m_playDifficulty]==oldClear,"Demo cannot set clear flags");StopDemo();}
  memset(g.m_bExtraPlayable,1,4);Check(StartDemo(true)&&Demo().number==5&&Demo().data.size()==40000,"hidden Extra loads its original first part");const auto seed=Demo().seed;DemoDialogueEnded();Check(Demo().extraPart2&&Demo().index==0&&Demo().data.size()==40000&&Demo().seed==seed,"Extra pre-boss dialogue swaps REC without reseeding");unsigned short input;Check(!DemoInput(32,input)&&!DemoInput(4096,input),"native shot and cancel terminate Demo");StopDemo(true);memcpy(g.m_bExtraPlayable,extras,4);
  srand(123);int expected=rand();srand(123);Check(Random()==expected,"ordinary games preserve their existing CRT RNG");
 }
 static void NativeIdle(){
  CTitleScreen title;title.Initialize(false,0,false);title.m_bMenuOn=true;title.m_bQuit=false;
  for(int i=0;i<639;i++)title.UpdateDemoIdle(0);Check(!title.m_bQuit,"native title waits 640 menu frames");
  title.UpdateDemoIdle(32);Check(!title.m_idleFrames,"native shot resets Demo idle");
  for(int i=0;i<640;i++)title.UpdateDemoIdle(0);Check(title.m_bQuit&&title.m_quitCode==TITLESCREEN_END_DEMO,"640 idle frames select normal Demo");
  title.m_bQuit=false;title.m_idleFrames=0;title.m_curMenu=1;
  for(int i=0;i<700;i++)title.UpdateDemoIdle(12);Check(!title.m_bQuit,"options can accrue idle time but cannot start Demo");
  title.m_curMenu=0;title.UpdateDemoIdle(12);Check(title.m_quitCode==TITLESCREEN_END_EXTRA_DEMO,"held Left+Right selects original hidden Extra shortcut");
 }
 struct VerdictBoss:CBossStage1 {void Collect(CStage* stage){m_pStage=stage;m_curPhase=0xff;m_curPhaseFrame=1;DeathSequence(1);}};
 static void Counters(CGame& game){using namespace original;auto& g=game.m_globalVar;
  g.m_bPracticeMode=g.m_bReplayMode=false;g.m_playChara=0;g.m_playDifficulty=1;g.m_playStage=0;g.m_nInitBomb=g.m_nInitLife=3;g.OnBeginGame();
  CStage* s=new CStage;s->Initialize();game.m_pCurScheme=s;game.m_curSchemeID=4;
  Check(Stats().frames==0&&Stats().itemsSpawned==0,"new normal run resets resident counters");
  for(int i=0;i<50;i++)s->m_pItem->AddItem(1000,1000,ITEM_POWER,0,0,false);
  Check(Stats().itemsSpawned==40,"full native item pool does not count failed spawns");
  s->SetDream(128);s->m_pChara->CaughtItem(1000,5000,ITEM_POINT);s->SetDream(1);s->m_pChara->CaughtItem(1000,5000,ITEM_POINT);
  Check(Stats().itemsCollected==2&&Stats().points==2&&Stats().maxPoints==1,"actual pickup hooks count yellow and white points separately");
  s->m_pChara->DropBomb();Check(Stats().bombs==1,"actual native bomb records resident usage");
  s->m_pChara->m_bCharaCanShoot=false;s->m_pChara->m_invulnerableTimeLeft=0;s->m_pChara->m_bMiss=true;for(int i=0;i<8;i++)s->m_pChara->Step();
  Check(Stats().misses==1,"actual native miss records resident usage");
  s->m_curNGraze=73;Check(!Stats().graze,"current stage graze is not yet resident total");
  g.m_playStage=5;{VerdictBoss boss;boss.Collect(s);}Check(Stats().graze==73,"boss post-death sequence transfers capped stage graze once");
  original::Frame(true,123);original::RenderTiming(true);original::PausedFrame();Check(Stats().stdFrames==1&&Stats().frames==2&&Stats().slowFrames==2,"native combat and pause counters retain independent totals");
  delete s;game.m_pCurScheme=NULL;
  g.m_playDifficulty=4;g.m_playStage=6;g.m_nInitLife=6;g.m_nInitBomb=0;g.OnBeginGame();Check(Stats().initialLives==3&&Stats().initialBombs==3,"Extra verdict uses forced original three-life three-bomb credit");
 }
 static void NativeMovement(CGame& game){using namespace original;StopDemo(true);Check(StartDemo(),"movement fixture starts native Demo");
  CStage* stage=new CStage;stage->Initialize();auto* player=stage->m_pChara;
  player->m_curX=3072;player->m_curY=4000;player->m_originalLastDirection=0;
  player->MoveChara(4);const float left=player->m_curX;const float speed=player->m_moveSpeed;
  player->MoveChara(12);Check(player->m_curX==left+speed&&player->m_originalLastDirection==4,"native opposing horizontal input retries against previous direction without replacing it");
  player->MoveChara(12);Check(player->m_curX==left+2*speed,"held opposing input repeats the native retry");
  player->MoveChara(0);const float stopped=player->m_curX;player->MoveChara(12);Check(player->m_curX==stopped,"opposing input without a previous direction stops movement");
  player->m_nextLifeComeTimeLeft=38;player->m_cannotControlTimeLeft=70;stage->m_nCurBomb=3;player->DropBomb();Check(!player->m_nextLifeComeTimeLeft&&player->m_cannotControlTimeLeft==0&&stage->m_nCurBomb==2,"native deathbomb consumes a bomb and clears death and movement lock");
  delete stage;StopDemo(true);
 }
 static void NativeMechanics(CGame& game){using namespace original;StopDemo(true);Check(StartDemo(),"native mechanic fixture starts Demo");
  ResetDemoSeed();Check(Demo().seed==0x5b18a0d7u&&Demo().itemDropIndex==8,"seed 318 reverse fill has native post-fill state");
  const unsigned char head[]={72,216,182,230,77,2,109,212};Check(!memcmp(Demo().ring,head,sizeof(head)),"native reverse ring fill matches reference bytes");
  Check(Random()==55368&&Demo().ringIndex==1,"native word RNG reads before increment");Demo().ringIndex=254;
  Check(Random()==11638&&Random()==65325&&Demo().ringIndex==0&&Random()==55368,"native RNG wraps with cursor byte as the last word high byte");
  Check(math::X(0,128)==128&&math::Y(64,128)==128&&math::X(128,128)==-128&&math::Y(192,128)==-128,"masterlib cardinal vectors");
  Check(math::X(65,1)==-1&&math::Y(255,1)==-1,"native signed fixed-point vectors round negative products down");
  Check(math::Angle(1,1)==32&&math::Angle(1,-1)==96&&math::Angle(-1,-1)==160&&math::Angle(-1,1)==224,"masterlib octant mapping");
  Check(math::Speed(255,64)==125&&math::Speed(1,0)==8&&math::Speed(128,64)==128,"native speed tuning wraps to byte before clamping");
  CStage* stage=new CStage;stage->Initialize();auto* player=stage->m_pChara;auto* bullets=stage->m_pCharaBullet;
  bullets->ShootOriginal(2,0,18,3072,4000);int count=0;for(int i=0;i<64;i++)count+=bullets->m_bullets[i].flag!=0;
  Check(count==1&&!bullets->m_bullets[0].flag&&bullets->m_bullets[1].damage==10&&bullets->m_bullets[1].velY==-192,"native basic shot skips slot zero and uses exact damage and velocity");
  bullets->Initialize();ResetDemoSeed();bullets->ShootOriginal(2,1,18,3072,4000);
  Check(Demo().ringIndex==1&&bullets->m_bullets[1].moveAngle==188&&bullets->m_bullets[1].velX==-19&&bullets->m_bullets[1].velY==-192,"native random basic shot uses one ring draw and masterlib vector");
  auto* enemyBullets=stage->m_pEnemyBullet;auto& b=enemyBullets->m_bullets[0];
  b.specialMoveType=EBMT03_SPEED_UP;b.curAngle=0;b.curSpeed=128;b.specialMoveVar=129;
  enemyBullets->SpecialMove(0);Check(b.velX==128&&b.curSpeed==1,"native acceleration calculates velocity before wrapping and adding the next speed");
  b.specialMoveType=EBMT09_SPEED_UP_VERTICAL;b.velY=100;b.specialMoveVar=3;b.curAngle=45;b.imgIdx=60;
  stage->m_curFrame=2;enemyBullets->SpecialMove(0);Check(b.velY==100,"native gravity does not accelerate on even stage frames");
  stage->m_curFrame=3;enemyBullets->SpecialMove(0);Check(b.velY==103&&b.curAngle==45&&b.imgIdx==60,"native gravity changes velocity on odd frames without rotating the sprite");
  b.specialMoveType=EBMT04_SLOW_DOWN_AND_TURN;b.curAngle=0;b.delta=128;b.curSpeed=31;
  enemyBullets->SpecialMove(0);Check(b.curAngle==224&&b.curSpeed==29,"native deceleration uses signed-byte shortest-angle division");
  b.specialMoveType=EBMT10_STRAIGHT;b.curAngle=1;b.curSpeed=1;b.origX=b.curX=1000;b.origY=b.curY=2000;b.totalDistanceTravelled=0;
  for(int i=0;i<64;i++){enemyBullets->SpecialMove(0);b.curX+=b.velX;b.curY+=b.velY;}
  Check(b.totalDistanceTravelled==64&&b.curX==1064&&b.curY==2001,"native exact linear motion quantizes total distance rather than per-frame velocity");
  for(int phase=0;phase<2;phase++){
   if(phase){stage->m_pBoss=new CBossStage1;stage->m_pBoss->m_curPhase=2;}
   stage->SetDream(128);for(int frame=0;frame<16;frame++){player->m_bombTimer=frame;player->DecreaseDreamInBomb();}
   Check(stage->GetDream()==(phase?124:120),"native dream drains every other frame in phase zero, every fourth in later boss phases");
  }
  delete stage;StopDemo(true);Check(StartDemo()&&StartDemo()&&StartDemo(),"Mima bomb fixture selects original third Demo");stage=new CStage;stage->Initialize();player=stage->m_pChara;
  ResetDemoSeed();stage->SetDream(128);player->m_bombTimer=0;for(int i=0;i<16;i++)player->StepBomb();
  Check(Demo().ringIndex==4&&player->m_bombTimer==16&&stage->GetDream()==120,"Mima native bomb frames zero through twelve consume four ring draws");
  delete stage;StopDemo(true);
 }
 static void HashWord(uint64_t& hash,uint32_t word){for(int i=0;i<4;i++){hash^=(word>>(i*8))&255;hash*=1099511628211ull;}}
 static uint64_t DemoDigest(CStage& stage,uint64_t hash){using namespace original;
  HashWord(hash,Demo().seed);HashWord(hash,Demo().ringIndex);HashWord(hash,Demo().index);HashWord(hash,Demo().extraPart2);
  HashWord(hash,stage.m_curFrame);HashWord(hash,int(stage.m_pChara->GetCurX()));HashWord(hash,int(stage.m_pChara->GetCurY()));
  HashWord(hash,stage.m_nCurLife);HashWord(hash,stage.m_nCurBomb);HashWord(hash,stage.m_curScore);HashWord(hash,stage.m_nMiss);HashWord(hash,stage.m_nBomb);
  HashWord(hash,stage.m_curDream);HashWord(hash,stage.m_playerPerformance);HashWord(hash,stage.m_pChara->GetPower());
  if(stage.m_pBoss){HashWord(hash,stage.m_pBoss->m_curPhase);HashWord(hash,stage.m_pBoss->m_curHP);HashWord(hash,stage.m_pBoss->m_curPhaseFrame);}
  return hash;
 }
 static void DemoSimulation(CGame& game){using namespace original;auto& g=game.m_globalVar;bool extras[4];memcpy(extras,g.m_bExtraPlayable,4);memset(g.m_bExtraPlayable,1,4);
  uint64_t expected[5]={};const int cadences[]={60,0,1};
  for(int pass=0;pass<3;pass++){StopDemo(true);for(int n=0;n<5;n++){
   Check(StartDemo(n==4),"original Demo initializes simulation");CStage* stage=new CStage;stage->Initialize();game.m_pCurScheme=stage;game.m_curSchemeID=4;
   int steps=0;uint64_t digest=14695981039346656037ull;
   while(!stage->m_bQuit&&steps<41000){stage->StepFrame(0);if(cadences[pass]&&steps%cadences[pass]==0)stage->Draw();digest=DemoDigest(*stage,digest);++steps;}
   printf("DEMO number=%d renderEvery=%d inputs=%d ticks=%d lives=%d misses=%d bombs=%d score=%d digest=%016llx\n",Demo().number,cadences[pass],Demo().index,steps,stage->m_nCurLife,stage->m_nMiss,stage->m_nBomb,stage->m_curScore,static_cast<unsigned long long>(digest));
   Check(stage->m_bQuit,"original input ends or native game-over exits safely; no survival requirement");
   if(!pass){expected[n]=digest;stage->Draw();glFinish();if(!n)Screenshot("original-demo.bmp");}
   else Check(expected[n]==digest,"entire Demo gameplay trace is independent of render frequency");
   CScheme* next=NULL;int id;Check(CSchemeSwitcher::SwitchScheme(&next,&id,4,stage->m_quitCode)&&id==2&&!DemoActive(),"all Demo exits return to title without registration");
   delete stage;delete next;game.m_pCurScheme=NULL;
  }}memcpy(g.m_bExtraPlayable,extras,4);
 }
 static int Run(){VerdictMath();CGame game;CGame::s_pCurGame=&game;Check(game.Initialize(false),"production game and OpenGL initialize");Scenes(game);Routes(game);NativeIdle();Counters(game);DemoData(game);NativeMovement(game);NativeMechanics(game);DemoSimulation(game);game.OnGameEnd();printf("PASS %d original-feature production checks\n",checks);return 0;}
};int PracticeTests::checks=0;
}
int main(){return th5w::PracticeTests::Run();}
