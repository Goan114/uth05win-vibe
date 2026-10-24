#include "../Game/Game.h"
#include "../Game/Original/OriginalScenes.h"
#include "../Game/TitleScreen/TitleScreen.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Stage/MidBoss/MidBoss.h"
#include "../Game/Stage/Background/Background.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/EnemyBullet/EnemyBullet.h"
#include "../Game/Stage/Enemy/Enemy.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Item/Item.h"
#include "../Game/Stage/Boss/BossStage1.h"
#include "../Game/Stage/Boss/BossStageEx.h"
#include "../Game/Stage/Laser/Laser.h"
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
 static void NativeEnemies(CGame& game){using namespace original;StopDemo(true);Check(StartDemo(),"enemy fixture starts native Demo");
  CStage* stage=new CStage;stage->Initialize();auto* enemies=stage->m_pEnemy;auto* player=stage->m_pChara;
  unsigned char table[]={1,0,1,0,0x70,0x3e,0x70,0x3e,0xfe,0,0,0,0};
  stage->m_curFrame=1;ResetDemoSeed();enemies->m_pEnemyTableReadPtr=table;enemies->m_noAddEnemy=true;
  enemies->StepEnemyTable();Check(Demo().ringIndex==0&&enemies->m_pEnemyTableReadPtr==table+11,"midboss skips random enemy coordinates while advancing STD");
  enemies->m_noAddEnemy=false;for(int i=0;i<ENEMY_N_RECORD;i++)enemies->m_enemy[i].flag=1;
  enemies->m_pEnemyTableReadPtr=table;enemies->StepEnemyTable();Check(Demo().ringIndex==0,"full enemy pool does not consume spawn RNG");
  memset(enemies->m_enemy,0,sizeof(enemies->m_enemy));enemies->m_pEnemyTableReadPtr=table;
  enemies->StepEnemyTable();Check(Demo().ringIndex==3&&enemies->m_enemy[0].curX==55368%(384*16)&&enemies->m_enemy[0].curY==46808%(368*16)&&enemies->m_enemy[0].autoShootTimer==182,"native spawn consumes X, Y, then autofire RNG in order");
  unsigned char wait[]={2,0,0,255};player->m_curX=192*16;player->m_curY=200*16;
  const int offsets[][2]={{-768,0},{767,0},{768,0},{-769,0},{0,-768},{0,767},{0,768},{0,-769}};
  const bool fires[]={false,false,true,true,false,false,true,true};
  for(int i=0;i<8;i++){
   memset(enemies->m_enemy,0,sizeof(enemies->m_enemy));auto& e=enemies->m_enemy[0];e.flag=1;e.pScript=wait;e.bAutoShoot=true;e.autoShootTimer=127;e.autoShootInterval=128;
   e.curX=player->m_curX+offsets[i][0];e.curY=player->m_curY+offsets[i][1];e.shootInfo.shootType=EST15_DEFAULT_SINGLE_TO_CHARA;e.shootInfo.bulletSpeed=42;
   enemies->Step();Check((e.autoShootTimer==0)==fires[i],"native autofire safe box includes left/top and excludes right/bottom");
  }
  delete stage;StopDemo(true);
 }
 static void NativeBeam(CGame& game){using namespace original;StopDemo(true);Check(StartDemo()&&StartDemo()&&Demo().number==2,"beam fixture selects Marisa Demo");
  CStage* stage=new CStage;stage->Initialize();auto* player=stage->m_pChara;
  stage->m_curFrame=1;player->m_bombTimer=16;player->m_curX=2000;player->m_curY=3000;
  Check(player->GetBombDamage(2000,3100,128,100,false)==22,"native Marisa beam includes target top edge above its center");
  Check(player->GetBombDamage(2000,3101,128,100,false)==0,"native Marisa beam excludes target one subpixel below player");
  Check(player->GetBombDamage(2128,3100,128,100,true)==22&&player->GetBombDamage(2129,3100,128,100,true)==0,"native Marisa beam includes horizontal hitbox edge");
  player->m_bombTimer=80;Check(player->GetBombDamage(2000,3100,128,100,false)==0,"native Marisa beam stops at frame eighty");
  delete stage;StopDemo(true);
 }
 static void NativeBossMechanics(CGame& game){using namespace original;StopDemo(true);Check(StartDemo(),"boss fixture starts native Demo");
  CStage* stage=new CStage;stage->Initialize();CBossStageEx boss;boss.m_pStage=stage;boss.Initialize();
  boss.m_curX=250*16;boss.m_curY=64*16;boss.m_imgGoLeft=186;boss.m_imgGoRight=184;boss.m_imgStay=180;
  ResetDemoSeed();boss.RandomWalk(0);Check(Demo().ringIndex==1&&boss.m_curX==4000+math::X(112,64)&&boss.m_curY==1024+math::Y(112,64),"native right-side flystep keeps masked angles and integer vectors");
  boss.m_curX=boss.m_curY=100;boss.MoveTo(85,85);Check(boss.m_curX==97&&boss.m_curY==93,"native return movement uses signed truncation and different X/Y divisors");
  boss.m_curX=3000;boss.m_curY=1200;int positions[5][2];ResetDemoSeed();for(int i=0;i<5;i++){positions[i][0]=3000-1024+(Random()&2047);positions[i][1]=1200-1024+(Random()&2047);}
  ResetDemoSeed();boss.DropItem();for(int i=0;i<5;i++)Check(stage->m_pItem->m_item[i].curX==positions[i][0]&&stage->m_pItem->m_item[i].curY==positions[i][1],"native boss drop draws X before Y independently of argument evaluation order");
  boss.m_curPhaseFrame=64;boss.m_demoPatternState=200;Demo().bulletTemplateAngle=17;Demo().ringIndex=0;
  boss.Phase6ShootType2();Check(boss.m_demoPatternState==57&&Demo().bulletTemplateAngle==20&&Demo().ringIndex==0,"native ring direction shares a byte with aimed attacks, with no random initial angle");
  boss.m_bFireWave[0]=false;boss.m_bFireWave[1]=true;boss.m_fireWaveY[1]=588;stage->m_pChara->m_bMiss=false;boss.StepFireWave();Check(!boss.m_bFireWave[1],"native firewave ends at screen row 608");
  delete stage;StopDemo(true);
 }
 static void NativeSpecialMechanics(CGame& game){using namespace original;StopDemo(true);Check(StartDemo(),"special-bullet fixture starts native Demo");
  CStage* stage=new CStage;stage->Initialize();stage->m_curFrame=2;
  CCurvingBullet curves;curves.m_pStage=stage;curves.Initialize();
  for(int i=0;i<8;i++)curves.AddBullet(1000,1000,0,6,11);
  Check(curves.m_bullet[6].flag&&!curves.m_bullet[7].flag,"native curved bullets have seven slots");
  curves.Step();auto& b=curves.m_bullet[0];Check(b.nodeX[0]==1000&&b.demoHeadX==1006&&b.demoHeadY==1000&&b.speed==5&&b.age==1,"native curved head advances separately from old-head trail node zero");
  b.flag=0;curves.AddBullet(1000,1000,0,4,11);Check(b.age==1,"reusing a native curved slot retains head age");
  b.nodeX[14]=400*16;curves.Step();Check(!b.flag,"native curved trail retires exactly on the expanded right boundary");
  stage->m_pLaser->Initialize();auto& ray=stage->m_pLaser->m_laser[0];
  ray.mode=LASER_MODE_SHOOT_OUT;ray.srcX=3197;ray.srcY=1049;ray.lenLower=3016;ray.lenUpper=5776;ray.angle=8;ray.width=6;
  stage->m_pLaser->FinishDemoFrame();Check(ray.mode==LASER_MODE_NONE,"native Extra frame 20187 frees a clipped inner ray before reusing its slot");
  ray.mode=LASER_MODE_SHOOT_OUT;ray.lenLower=256;ray.lenUpper=1000;ray.angle=64;
  stage->m_pLaser->FinishDemoFrame();Check(ray.mode==LASER_MODE_SHOOT_OUT,"native clipping retains a ray inside the playfield");
  delete stage;StopDemo(true);
 }
 static void HashWord(uint64_t& hash,uint32_t word){for(int i=0;i<4;i++){hash^=(word>>(i*8))&255;hash*=1099511628211ull;}}
 static uint64_t DemoDigest(CStage& stage,uint64_t hash){using namespace original;
  HashWord(hash,Demo().seed);HashWord(hash,Demo().ringIndex);HashWord(hash,Demo().index);HashWord(hash,Demo().extraPart2);
  HashWord(hash,stage.m_curFrame);HashWord(hash,int(stage.m_pChara->GetCurX()));HashWord(hash,int(stage.m_pChara->GetCurY()));
  HashWord(hash,stage.m_nCurLife);HashWord(hash,stage.m_nCurBomb);HashWord(hash,stage.m_curScore);HashWord(hash,stage.m_nMiss);HashWord(hash,stage.m_nBomb);
  HashWord(hash,stage.m_curDream);HashWord(hash,stage.m_playerPerformance);HashWord(hash,stage.m_pChara->GetPower());
  if(stage.m_pBoss){HashWord(hash,stage.m_pBoss->m_curPhase);HashWord(hash,stage.m_pBoss->m_curHP);HashWord(hash,stage.m_pBoss->m_curPhaseFrame);}
  return SpecialFrame(stage,hash);
 }
 static uint64_t CombatFrame(CStage& stage,uint64_t hash){using namespace original;
  auto* p=stage.m_pChara;
  const uint32_t fields[]={uint32_t(stage.m_curFrame),uint32_t(p->m_curX),uint32_t(p->m_curY),Demo().seed,uint32_t(Demo().ringIndex),uint32_t(p->GetPower()),uint32_t(stage.GetDream()),uint32_t(stage.m_nCurLife),uint32_t(stage.m_nCurBomb),uint32_t(stage.m_playerPerformance),uint32_t(p->m_bMiss),uint32_t(p->m_nextLifeComeTimeLeft),uint32_t(p->m_invulnerableTimeLeft),uint32_t(p->m_cannotControlTimeLeft)};
  for(auto word:fields)HashWord(hash,word);
  int count=0;for(int i=0;i<40;i++)if(stage.m_pItem->m_item[i].flag==1)++count;HashWord(hash,count);
  for(int i=0;i<40;i++){const auto& v=stage.m_pItem->m_item[i];if(v.flag!=1)continue;const int words[]={i,v.flag,int(v.curX),int(v.curY),int(v.velX),int(v.velY),v.itemType};for(auto word:words)HashWord(hash,word);}
  count=0;for(int i=0;i<32;i++)if(stage.m_pEnemy->m_enemy[i].flag==1)++count;HashWord(hash,count);
  for(int i=0;i<32;i++){const auto& v=stage.m_pEnemy->m_enemy[i];if(v.flag!=1)continue;const int words[]={i,v.flag,v.life,int(v.curX),int(v.curY),int(v.velX),int(v.velY),v.autoShootTimer};for(auto word:words)HashWord(hash,word);}
  count=0;for(int i=0;i<400;i++)if(stage.m_pEnemyBullet->m_bullets[i<180?i:1800+i-180].flag==1)++count;HashWord(hash,count);
  for(int i=0;i<400;i++){const auto& v=stage.m_pEnemyBullet->m_bullets[i<180?i:1800+i-180];if(v.flag!=1)continue;const int words[]={i,int(v.curX),int(v.curY),int(v.velX),int(v.velY),int(v.curSpeed),int(v.curAngle),v.bornStageAndGrazeFlag,v.dieStageAndMoveType,v.age};for(auto word:words)HashWord(hash,word);}
  return hash;
 }
 static uint64_t SpecialFrame(CStage& stage,uint64_t hash){
  int count=0;for(const auto& v:stage.m_pLaser->m_laser)if(v.mode)++count;HashWord(hash,count);
  for(int i=0;i<32;i++){const auto& v=stage.m_pLaser->m_laser[i];if(!v.mode)continue;const int words[]={i,int(v.mode),int(v.srcX),int(v.srcY),int(v.lenLower),int(v.lenUpper),int(v.angle)&255,int(v.width),int(v.speed),v.age,v.ageThres};for(int word:words)HashWord(hash,word);}
  count=0;const CBossStageEx* boss=stage.m_pBoss&&stage.m_pBoss->m_curPhase&&CGame::GVar().m_playStage==6?static_cast<const CBossStageEx*>(stage.m_pBoss):NULL;
  if(boss)for(int i=0;i<7;i++)if(boss->m_curvingBullet.m_bullet[i].flag)++count;HashWord(hash,count);
  if(boss)for(int i=0;i<7;i++){const auto& v=boss->m_curvingBullet.m_bullet[i];if(!v.flag)continue;const int words[]={i,v.mode,v.demoHeadX,v.demoHeadY,int(v.velX),int(v.velY),int(v.angle),int(v.speed),v.age};for(int word:words)HashWord(hash,word);for(int k=0;k<16;k++){HashWord(hash,int(v.nodeX[k]));HashWord(hash,int(v.nodeY[k]));}}
  return hash;
 }
 static void WriteDemoTrace(CStage& stage,FILE* trace){using namespace original;
  auto* p=stage.m_pChara;
  fprintf(trace,"{\"frame\":%d,\"x\":%d,\"y\":%d,\"seed\":%u,\"ring\":%d,\"power\":%d,\"dream\":%d,\"lives\":%d,\"bombs\":%d,\"rank\":%d,\"hit\":%d,\"miss\":%d,\"inv\":%d,\"lock\":%d,\"items\":[",stage.m_curFrame,int(p->m_curX),int(p->m_curY),Demo().seed,Demo().ringIndex,p->GetPower(),stage.GetDream(),stage.m_nCurLife,stage.m_nCurBomb,stage.m_playerPerformance,int(p->m_bMiss),p->m_nextLifeComeTimeLeft,p->m_invulnerableTimeLeft,p->m_cannotControlTimeLeft);bool comma=false;
  for(int j=0;j<40;j++){const auto& v=stage.m_pItem->m_item[j];if(v.flag){fprintf(trace,"%s[%d,%d,%d,%d,%d,%d,%d]",comma?",":"",j,v.flag,int(v.curX),int(v.curY),int(v.velX),int(v.velY),v.itemType);comma=true;}}
  fprintf(trace,"],\"enemies\":[");comma=false;
  for(int j=0;j<32;j++){const auto& v=stage.m_pEnemy->m_enemy[j];if(v.flag){fprintf(trace,"%s[%d,%d,%d,%d,%d,%d,%d,%d]",comma?",":"",j,v.flag,v.life,int(v.curX),int(v.curY),int(v.velX),int(v.velY),v.autoShootTimer);comma=true;}}
  fprintf(trace,"],\"bullets\":[");comma=false;
  for(int j=0;j<400;j++){const auto& v=stage.m_pEnemyBullet->m_bullets[j<180?j:1800+j-180];if(v.flag==1){fprintf(trace,"%s[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",comma?",":"",j,int(v.curX),int(v.curY),int(v.velX),int(v.velY),int(v.curSpeed),int(v.curAngle),v.bornStageAndGrazeFlag,v.dieStageAndMoveType,v.age);comma=true;}}
  fprintf(trace,"],\"lasers\":[");comma=false;
  for(int j=0;j<32;j++){const auto& v=stage.m_pLaser->m_laser[j];if(v.mode){fprintf(trace,"%s[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",comma?",":"",j,int(v.mode),int(v.srcX),int(v.srcY),int(v.lenLower),int(v.lenUpper),int(v.angle)&255,int(v.width),int(v.speed),v.age,v.ageThres);comma=true;}}
  fprintf(trace,"],\"curves\":[");comma=false;
  if(stage.m_pBoss&&stage.m_pBoss->m_curPhase&&CGame::GVar().m_playStage==6){const auto* boss=static_cast<const CBossStageEx*>(stage.m_pBoss);
   for(int j=0;j<7;j++){const auto& v=boss->m_curvingBullet.m_bullet[j];if(v.flag){fprintf(trace,"%s[%d,%d,%d,%d,%d,%d,%d,%d,%d",comma?",":"",j,v.mode,v.demoHeadX,v.demoHeadY,int(v.velX),int(v.velY),int(v.angle),int(v.speed),v.age);for(int k=0;k<16;k++)fprintf(trace,",%d,%d",int(v.nodeX[k]),int(v.nodeY[k]));fprintf(trace,"]");comma=true;}}
  }
  fprintf(trace,"],\"boss\":[%d,%d,%d,%d,%d,%d],\"scroll\":[%d,%d],\"midboss\":[%d,%d,%d,%d,%d]}\n",stage.m_pBoss?int(stage.m_pBoss->m_curX):0,stage.m_pBoss?int(stage.m_pBoss->m_curY):0,stage.m_pBoss?stage.m_pBoss->m_curHP:0,stage.m_pBoss?stage.m_pBoss->m_curPhase:0,stage.m_pBoss?stage.m_pBoss->m_curPhaseFrame:0,stage.m_pBoss?stage.m_pBoss->m_curMode:0,stage.m_pBackground->GetCurScrollSpeed(),stage.m_pBackground->GetLastScrollDeltaPixel()*16,stage.m_pMidBoss?int(stage.m_pMidBoss->m_curX):0,stage.m_pMidBoss?int(stage.m_pMidBoss->m_curY):0,stage.m_pMidBoss?stage.m_pMidBoss->m_curHP:0,stage.m_pMidBoss?stage.m_pMidBoss->m_curPhase:0,stage.m_pMidBoss?stage.m_pMidBoss->m_curPhaseFrame:0);
 }
 static void DemoSimulation(CGame& game){using namespace original;auto& g=game.m_globalVar;bool extras[4];memcpy(extras,g.m_bExtraPlayable,4);memset(g.m_bExtraPlayable,1,4);
  uint64_t expected[5]={};const int cadences[]={60,0,1};
  for(int pass=0;pass<(getenv("TH05_DEMO_ONLY")?1:3);pass++){StopDemo(true);for(int n=getenv("TH05_DEMO_ONLY")?4:0;n<5;n++){
   Check(StartDemo(n==4),"original Demo initializes simulation");CStage* stage=new CStage;stage->Initialize();game.m_pCurScheme=stage;game.m_curSchemeID=4;
   int steps=0;uint64_t digest=14695981039346656037ull;
   FILE* trace=NULL;if(!pass&&getenv("TH05_DEMO_TRACE")){char path[80];sprintf(path,"port-demo-%d.jsonl",n+1);trace=fopen(path,"wb");Check(trace!=NULL,"Demo trace output opens successfully");}
   uint64_t combat=14695981039346656037ull,special=combat;int lastCombatFrame=-1;
   while(!stage->m_bQuit&&steps<41000){
    stage->StepFrame(0);
    if(stage->m_curFrame>=0&&stage->m_curFrame!=lastCombatFrame){combat=CombatFrame(*stage,combat);special=SpecialFrame(*stage,special);lastCombatFrame=stage->m_curFrame;}
    if(trace&&stage->m_curFrame>=0)WriteDemoTrace(*stage,trace);
    if(cadences[pass]&&steps%cadences[pass]==0)stage->Draw();
    digest=DemoDigest(*stage,digest);++steps;
   }
   const uint64_t nativeGolden[]={0xcfa52d7b55bc18c4ull,0xed12e228ceec92e1ull,0xc3e0677110a1485eull,0x93747c3bbd75f739ull};
   if(n<4)Check(lastCombatFrame==4995&&combat==nativeGolden[n],"every normal Demo combat frame matches the original Japanese instructions");
   else Check(lastCombatFrame==26217&&combat==0x3a135d8958738d5full,"Extra combat and native title-return endpoint match the original instructions");
   Check(special==(n<4?0xbf207814eb9925a5ull:0xedc31a317fc555ddull),"all native laser slots and curved heads/trail nodes match at every frame");
   printf("SPECIAL number=%d digest=%016llx\n",n+1,static_cast<unsigned long long>(special));
   printf("COMBAT number=%d frames=%d digest=%016llx\n",n+1,lastCombatFrame+1,static_cast<unsigned long long>(combat));
   if(trace)fclose(trace);
   printf("DEMO number=%d renderEvery=%d inputs=%d ticks=%d lives=%d misses=%d bombs=%d score=%d digest=%016llx\n",Demo().number,cadences[pass],Demo().index,steps,stage->m_nCurLife,stage->m_nMiss,stage->m_nBomb,stage->m_curScore,static_cast<unsigned long long>(digest));
   Check(stage->m_bQuit,"original input ends or native game-over exits safely; no survival requirement");
   if(!pass){expected[n]=digest;stage->Draw();glFinish();if(!n)Screenshot("original-demo.bmp");}
   else Check(expected[n]==digest,"entire Demo gameplay trace is independent of render frequency");
   CScheme* next=NULL;int id;Check(CSchemeSwitcher::SwitchScheme(&next,&id,4,stage->m_quitCode)&&id==2&&!DemoActive(),"all Demo exits return to title without registration");
   delete stage;delete next;game.m_pCurScheme=NULL;
  }}memcpy(g.m_bExtraPlayable,extras,4);
 }
 static int Run(){VerdictMath();CGame game;CGame::s_pCurGame=&game;Check(game.Initialize(false),"production game and OpenGL initialize");if(getenv("TH05_DEMO_ONLY")){DemoSimulation(game);game.OnGameEnd();return 0;}Scenes(game);Routes(game);NativeIdle();Counters(game);DemoData(game);NativeMovement(game);NativeMechanics(game);NativeEnemies(game);NativeBeam(game);NativeBossMechanics(game);NativeSpecialMechanics(game);DemoSimulation(game);game.OnGameEnd();printf("PASS %d original-feature production checks\n",checks);return 0;}
};int PracticeTests::checks=0;
}
int main(){return th5w::PracticeTests::Run();}
