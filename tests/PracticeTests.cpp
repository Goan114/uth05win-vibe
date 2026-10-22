#include "../Game/Game.h"
#include "../Game/Practice/Practice.h"
#include "../Game/Stage/Stage.h"
#include "../Game/SchemeSwitcher.h"
#include "../Game/MusicRoom/MusicRoom.h"
#include "../Game/MusicRoom/MusicTracks.h"
#include "../Game/SelectReplayScreen/SelectReplayScreen.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")
#include "../Game/TitleScreen/TitleScreen.h"
#include "../Game/SelectCharaScreen/SelectCharaScreen.h"
#include "../Game/Stage/Chara/Chara.h"
#include "../Game/Stage/Dialogue/Dialogue.h"
#include "../Game/Stage/Item/Item.h"
#include "../Game/Stage/Boss/BossStage1.h"
#include "../Game/Stage/MidBoss/MidBossStage1.h"
#include "../Game/Stage/CharaBullet/CharaBullet.h"
#include "../Game/Stage/EnemyBullet/EnemyBullet.h"
#include "../Game/Stage/FloatingText/FloatingText.h"
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_internal.h"
#include <cstdio>
#include <cstdlib>
#include <new>
#include <vector>
#include <gl/GL.h>
namespace th5w {
struct PracticeTests {
 static int checks;
 static void Check(bool ok,const char* reason) { ++checks; if (!ok) { fprintf(stderr,"FAIL: %s\n",reason); exit(1); } }
 static void Key(CGame& game,int key,bool repeat=false) { practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,key,repeat?(1L<<30):0); practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,key,1L<<31); }
 static void ResetTools() { for(int i=0;i<practice::ToolCount;i++) if(practice::Enabled(practice::Tool(i))) practice::Toggle(practice::Tool(i)); }
 static CStage* Stage(CGame& game) {
  CGame::GVar().m_playStage=0; CGame::GVar().m_playChara=0; CGame::GVar().m_playDifficulty=1;
  CGame::GVar().m_bReplayMode=false; CGame::GVar().m_bPracticeMode=false;
  CGame::GVar().OnBeginGame(); CStage* s=new CStage; s->Initialize();
  s->m_bLogo=false; s->m_bCanPause=true; s->m_bInCharaBomb=false;
  s->m_pChara->m_invulnerableTimeLeft=0;s->m_pChara->m_bCharaCanShoot=false;
  s->m_pChara->m_cannotControlTimeLeft=0;s->m_curGameKeyState=s->m_lastGameKeyState=0;
  game.m_pCurScheme=s;game.m_curSchemeID=4;return s;
 }
 static void Screenshot(const char* name="pause.bmp") {
  std::vector<unsigned char> pixels(640*480*3);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,640,480,GL_BGR_EXT,GL_UNSIGNED_BYTE,&pixels[0]);
  BITMAPFILEHEADER f={};f.bfType=0x4d42;f.bfOffBits=sizeof(f)+sizeof(BITMAPINFOHEADER);f.bfSize=f.bfOffBits+(DWORD)pixels.size();
  BITMAPINFOHEADER h={};h.biSize=sizeof(h);h.biWidth=640;h.biHeight=480;h.biPlanes=1;h.biBitCount=24;
  FILE* out=fopen(name,"wb");fwrite(&f,sizeof(f),1,out);fwrite(&h,sizeof(h),1,out);fwrite(&pixels[0],pixels.size(),1,out);fclose(out);
 }
 static void Frame(CStage& s) { practice::BeginFrame();s.Draw();practice::DrawStatus();practice::EndFrame();glFinish(); }
 static void CheckPracticeHud(CStage& stage) {
  Check(strcmp(stage.m_pStageRes->m_crossRow,"\x81\x40\x81\x40\x81\x7e\x81\x40\x81\x40")==0,"unmodified resources initialize the native life/bomb multiplication label");
  Frame(stage);
  std::vector<unsigned char> pixels(24*480*3);
  glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,24,480,GL_RGB,GL_UNSIGNED_BYTE,&pixels[0]);
  int lit=0;for(size_t i=0;i<pixels.size();i++)if(pixels[i]>32)++lit;
  if(lit) {fprintf(stderr,"Practice HUD left-margin components: %d\n",lit);Screenshot("practice-hud-regression.bmp");}
  Check(lit==0,"Practice HUD must not draw stray multiplication glyphs in the left margin");
  Screenshot("practice-hud-fixed.bmp");
 }
 static void InactiveFloatingText(CStage& stage) {
  alignas(CFloatingText) unsigned char storage[sizeof(CFloatingText)]={};
  CFloatingText* text=new(storage) CFloatingText(&stage);
  for(int cycle=0;cycle<2;cycle++) {
   for(int frame=0;frame<256;frame++) {
    glColor4f(.25f,.5f,.75f,1);text->Step();text->Draw();
    float color[4];glGetFloatv(GL_CURRENT_COLOR,color);
    Check(color[0]==.25f&&color[1]==.5f&&color[2]==.75f,"inactive floating text never draws a glyph or changes render state");
   }
   text->SetTextType(FLOATING_TEXT_BONUS,12345);
   for(int i=0;i<12;i++)text->Step();
   text->Initialize();
  }
  text->~CFloatingText();
 }
 static void NativePractice(CGame& game) {
  ResetTools(); auto& g=CGame::GVar();
  g.m_replay.CloseStage(false);
  CTitleScreen title;title.Initialize(false,2,false);title.m_bMenuOn=true;title.m_curScrFade=100;title.m_bgFade=100;
  Check(ImmGetContext(game.m_gameWindow.m_hWnd)==NULL,"game window has no IME composition context");
  Check(title.m_mainMenuNItem==8&&title.m_bMainMenuItemEnabled[2],"Practice Start is the third native title item");
  title.Draw();glFinish();Screenshot("practice-title.bmp");

  CScheme* flow=NULL;int flowID=0;
  memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));
  memset(g.m_bClear,0,sizeof(g.m_bClear));
  g.m_playChara=0;g.m_playDifficulty=1;g.m_playStage=0;
  g.m_bPracticeMode=false;g.m_bReplayMode=false;practice::ResetRun();
  Check(!g.PracticeStageUnlocked(0),"uncleared stages start locked");
  g.OnBeginGame();CStage* entered=new CStage;entered->Initialize();
  Check(g.PracticeStageUnlocked(0)&&!g.PracticeStageUnlocked(1),"stage initialization unlocks its Practice before the first gameplay frame");
  g.m_replay.CloseStage(false);delete entered;
  g.m_playChara=1;Check(!g.PracticeStageUnlocked(0),"stage unlock is character-specific");
  g.m_playChara=0;g.m_playDifficulty=2;Check(!g.PracticeStageUnlocked(0),"stage unlock is difficulty-specific");
  g.m_playDifficulty=1;g.m_playStage=1;g.m_bPracticeMode=true;g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"Practice entry cannot unlock stages");
  g.m_bPracticeMode=false;g.m_bReplayMode=true;g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"Replay entry cannot unlock stages");
  g.m_bReplayMode=false;Key(game,VK_F1);g.OnStageEnter();Check(!g.PracticeStageUnlocked(1),"assisted entry cannot unlock stages");ResetTools();practice::ResetRun();
  memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));g.LoadPracticeProgress();
  Check(g.PracticeStageUnlocked(0)&&!g.PracticeStageUnlocked(1),"unlock progress persists across reload");
  { FILE* cfg=fopen("game.cfg","rb");Check(cfg!=NULL,"unlock uses native game.cfg");
    fseek(cfg,0,SEEK_END);Check(ftell(cfg)==92,"native config has its 52-byte prefix and versioned progress extension");fclose(cfg); }
  g.LoadConfig();g.LoadPracticeProgress();Check(g.PracticeStageUnlocked(0),"native configuration reload preserves entry unlock");
  g.m_bClear[2][3]=true;g.LoadPracticeProgress();g.m_playChara=2;g.m_playDifficulty=3;
  Check(g.PracticeStageUnlocked(5),"existing full-game clear migrates completed stages");g.m_bClear[2][3]=false;
  g.m_bPracticeMode=true;g.m_bReplayMode=false;g.m_playChara=0;g.m_playDifficulty=1;g.m_playStage=0;

  Check(CSchemeSwitcher::SwitchScheme(&flow,&flowID,2,TITLESCREEN_END_START_PRACTICE)&&flowID==3,"Practice Start enters native character select");
  CSelectCharaScreen* select=static_cast<CSelectCharaScreen*>(flow);
  for(int i=0;i<4;i++)Check(select->m_bCharaSelectable[i],"Practice uses all normal character choices");
  select->m_bStageSelect=true;select->m_stageSelectAge=30;select->m_curScrFade=100;
  select->ParseStageKey(1);Check(select->m_stageCursor==5,"Practice stage list wraps upward to stage six");
  select->ParseStageKey(0);select->ParseStageKey(16);
  Check(select->m_bStageSelect&&select->m_stageSelectClosing&&!select->m_bQuit,"X begins closing without instantly removing the window");
  select->Step();Check(select->m_bStageSelect&&select->m_stageSelectAge==9,"closing animation moves inward on next frame");
  select->Draw();glFinish();Screenshot("practice-close-09.bmp");
  for(int frame=0;frame<9;frame++) select->Step();
  Check(!select->m_bStageSelect&&!select->m_stageSelectClosing,"closing returns to character selection after ten frames");
  select->m_bStageSelect=true;select->m_lastKeyState=0;select->m_stageSelectAge=30;
  select->Draw();glFinish();Screenshot("practice-stage.bmp");
  const int animationFrames[]={0,2,5,10};
  for(int n=0;n<4;n++) {
   select->m_stageSelectAge=animationFrames[n];
   select->Draw();glFinish();char path[64];sprintf_s(path,"practice-expand-%02d.bmp",animationFrames[n]);Screenshot(path);
  }
  select->m_stageSelectAge=30;
  select->ParseStageKey(32);
  Check(!select->m_bQuit,"confirming a locked stage does not start gameplay");
  g.m_practiceUnlocked[0][1]|=1<<5;
  select->ParseStageKey(0);select->ParseStageKey(32);
  Check(select->m_bQuit&&g.m_playStage==5&&g.m_bPracticeMode,"native stage confirmation starts selected stage");
  Check(g.m_nCurLife==9&&g.m_nCurBomb==8&&g.m_curPower==128,"Practice starts with eight spare lives, eight bombs and full power");
  delete select;
  // Exercise real production initialization, input stream, disk round trip,
  // playback state and end routing independently for every selectable stage.
  for(int stage=0;stage<6;stage++) {
   g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=stage;g.m_playChara=stage%4;g.OnBeginGame();
   CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;
   Check(record->m_nCurLife==9&&record->m_nCurBomb==8,"per-stage native resources are 8 spare lives / 8 bombs");
   std::vector<double> states;
   for(int frame=0;frame<1200;frame++) {
    unsigned short input=32 | ((frame/60)%2?64:0) | ((frame/90)%2?4:8);
    record->StepFrame(input);
    states.push_back(record->m_curFrame);states.push_back(record->m_curScore);
    states.push_back(record->m_pChara->m_curX);states.push_back(record->m_pChara->m_curY);
    states.push_back(record->m_nCurLife);states.push_back(record->m_nCurBomb);
    states.push_back(record->m_pChara->m_power);
   }
   CheckPracticeHud(*record);
   g.m_bCanSaveReplay=false; // A stale menu flag must not suppress a completed Practice save.
   record->SetQuit(STAGE_END_CLEAR);int end=0;for(int f=0;f<60&&!end;f++)end=record->Step();
   Check(end==STAGE_END_CLEAR&&g.m_replay.m_stageFlag[stage]==2,"single-stage clear closes recorded stream as cleared");
   Check(g.m_bCanSaveReplay&&g.m_replay.m_bPracticeMode,"normal Practice remains replay-save eligible");
   Check(g.m_replay.SaveFile("practice-roundtrip.rpy"),"Practice replay writes native file format");
   REPLAY_BRIEF_INFO info={};Check(CReplay::GetRepFileInfo(&info,"practice-roundtrip.rpy")&&info.stageFlag[stage]==2,"saved Practice is selectable in native replay browser");
   CScheme* save=NULL;int saveID=0;
   Check(CSchemeSwitcher::SwitchScheme(&save,&saveID,4,end)&&saveID==6&&g.m_playStage==stage,"single-stage clear goes directly to save replay without next stage");{
    CSelectReplayScreen* page=static_cast<CSelectReplayScreen*>(save);
    Check(!page->m_bQuit&&!page->m_bLoadMode,"Practice clear presents an active save page");
    page->m_bEnterNameMode=true;page->m_curRowY=(page->m_listUpperLeftY+4)*16;
    page->m_curCharX=12;page->m_curCharY=6;page->m_selectedSlot=stage;
    memset(page->m_curEnterName,0,10);memcpy(page->m_curEnterName,"PLAYER",6);
    page->m_lastKeyState=0;page->m_curKeyState=32;page->StepEnterNameMode();
    char saved[64];sprintf_s(saved,"replay/replay%02d.rpy",stage);REPLAY_BRIEF_INFO savedInfo={};
    Check(CReplay::GetRepFileInfo(&savedInfo,saved)&&savedInfo.stageFlag[stage]==2,"native name-grid END actually saves the completed Practice replay");
   }delete save;delete record;
   Check(g.m_replay.LoadFile("practice-roundtrip.rpy"),"native Practice replay reloads");
   g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();
   CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;
   bool equal=true;size_t n=0;
   for(int frame=0;frame<1200;frame++) {
    playback->StepFrame(0);
    double state[]={double(playback->m_curFrame),double(playback->m_curScore),double(playback->m_pChara->m_curX),double(playback->m_pChara->m_curY),double(playback->m_nCurLife),double(playback->m_nCurBomb),double(playback->m_pChara->m_power)};
    for(int i=0;i<7;i++)if(state[i]!=states[n++])equal=false;
   }
   Check(equal,"saved Practice playback matches every recorded frame state");
   playback->SetQuit(STAGE_END_CLEAR);end=0;for(int f=0;f<60&&!end;f++)end=playback->Step();
   Check(end==STAGE_END_REPLAY_END,"Practice playback clear terminates rather than advancing stages");
   delete playback;game.m_pCurScheme=NULL;
  }
  g.m_bReplayMode=false;g.m_bPracticeMode=true;g.m_playStage=2;g.OnBeginGame();
  CScheme* save=NULL;int id=0;
  Check(CSchemeSwitcher::SwitchScheme(&save,&id,4,STAGE_END_ALL_MISSED)&&id==6,"Practice game over also goes straight to replay save");delete save;
  bool oldClear=g.m_bClear[g.m_playChara][g.m_playDifficulty];g.m_bClear[g.m_playChara][g.m_playDifficulty]=false;g.OnClear();
  Check(!g.m_bClear[g.m_playChara][g.m_playDifficulty],"Practice does not unlock normal-game clears");g.m_bClear[g.m_playChara][g.m_playDifficulty]=oldClear;
 }
 static void CustomPractice(CGame& game){
  ResetTools();auto& g=CGame::GVar();g.m_replay.CloseStage(false);g.m_bReplayMode=false;g.m_initDifficulty=1;
  practice::OpenSetup(0);auto& menu=practice::MenuConfig();
  menu=practice::Config();menu.mode=1;menu.warp=3;menu.frame=1000;menu.lockRank=1;menu.rank=48;
  CSelectCharaScreen setup;setup.Initialize();setup.m_curScrFade=100;setup.m_bStageSelect=setup.m_bThpracSetup=true;setup.m_stageSelectAge=20;
  for(int i=0;i<12;i++){practice::BeginFrame();setup.Draw();practice::EndFrame();glFinish();}
  Screenshot("thprac-custom-menu.bmp");
  ImGuiWindow* window=ImGui::FindWindowByName("TH05 Practice");
  Check(window&&window->Size.x==330&&window->Size.y==430,"custom menu uses purple thprac's borderless ImGui layout");
  Check(window->ScrollMax.y==0,"all custom fields fit without clipping");
  Check(ImGui::GetFont()->FindGlyphNoFallback(0x5e27)!=NULL,"frame glyph is present instead of falling back to a question mark");
  const auto setupKey=[&](int key){
   practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,key,0);
   practice::BeginFrame();setup.Draw();practice::EndFrame();const int action=setup.m_setupAction;
   practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,key,1L<<31);
   practice::BeginFrame();setup.Draw();practice::EndFrame();return action;
  };
  ImGui::FocusWindow(window);ImGui::SetFocusID(window->GetID(u8"\u5173\u5361"),window);GImGui->NavDisableHighlight=false;
  Check(setupKey(VK_RIGHT)==0&&menu.stage==1,"focused combo uses purple left/right adjustment without starting");
  menu.stage=0;
  Check(setupKey('Z')==1,"Z starts directly from a focused parameter without an invented button");
  Check(!ImGui::IsPopupOpen(static_cast<const char*>(NULL),ImGuiPopupFlags_AnyPopupId),"Z does not also open the parameter combo");
  Check(setupKey('X')==2,"X requests return directly from the practice menu");
  setup.m_setupAction=2;setup.Step();Check(setup.m_stageSelectClosing&&setup.m_stageSelectAge==8,"X begins the purple menu's fade-out");
  for(int i=0;i<8;i++)setup.Step();Check(!setup.m_bThpracSetup&&!setup.m_bQuit,"X returns to character selection after fade-out");
  setup.m_bThpracSetup=setup.m_bStageSelect=true;setup.m_stageSelectAge=30;
  practice::SetAdvancedOpen(true);Check(setupKey('Z')==0,"F12 prevents hidden practice parameters from consuming Z");practice::SetAdvancedOpen(false);
  menu.stage=5;menu.warp=1;Check(practice::StartSetup(),"stage six midboss selection is a valid start-of-stage fallback");
  setup.m_bThpracSetup=setup.m_bStageSelect=true;setup.m_stageSelectClosing=false;setup.m_stageSelectAge=30;
  practice::BeginFrame();setup.Draw();practice::EndFrame();Screenshot("thprac-stage6-no-mid.bmp");
  menu.mode=0;menu.stage=0;memset(g.m_practiceUnlocked,0,sizeof(g.m_practiceUnlocked));g.m_playChara=0;g.m_playDifficulty=1;
  Check(!practice::StartSetup(),"normal mode still obeys native stage unlocks");
  // Every supported stage / destination round-trips through the real disk replay.
  for(int stage=0;stage<7;stage++)for(int warp=0;warp<4;warp++){
   menu=practice::Config();menu.mode=1;menu.stage=stage;menu.warp=warp;menu.frame=1000;
   menu.lives=3;menu.bombs=2;menu.score=123450;menu.power=69;menu.graze=321;menu.points=123;menu.totalPoints=4567;menu.dream=70;menu.rank=48;menu.lockRank=1;
   g.m_bReplayMode=false;g.m_playChara=stage%4;
   Check(practice::StartSetup(),"custom mode starts locked stages and Extra independently of native unlocks");
   Check(g.m_nCurLife==4&&g.m_nCurBomb==2&&g.m_curPower==69&&g.m_curScore==12345&&g.m_curDream==70,"all configured initial resources use native units");
   Check(g.m_totalNPointItem==4567&&g.m_curNGraze==321&&g.m_playerPerformance==48&&g.m_bCanSaveReplay,"custom counters and rank preserve replay eligibility");
   CStage* record=new CStage;record->Initialize();game.m_pCurScheme=record;game.m_curSchemeID=4;
   Check(record->m_curNGraze==321&&record->m_curNPointItem==123&&record->m_totalNPointItem==4567,"HUD counters receive custom graze and blue points");
   Check(record->m_playerPerformanceLowerBound==48&&record->m_playerPerformanceUpperBound==48,"locked rank constrains both rank bounds");
   record->ChangePlayerPerformance(-100);record->ChangePlayerPerformance(100);
   Check(record->m_playerPerformance==48,"locked rank rejects increases and decreases");
   if(warp==1&&stage!=5){const int mid[]={2500,2750,5750,3900,4800,0,5800};Check(record->m_curFrame==mid[stage]-1,"midboss seek stops immediately before the native spawn frame");}
   if(warp==1&&stage==5)Check(record->m_curFrame==-1&&record->m_bLogo&&record->m_pMidBoss==NULL,"sixth-stage missing midboss starts from the original beginning");
   if(warp==2)Check(record->m_pBoss!=NULL&&!record->m_pDialogue->IsRunning(),"boss warp executes original dialogue resource loading and enters the real boss");
   if(warp==3)Check(record->m_curFrame==999,"frame warp begins at the selected timeline frame");
   Frame(*record);char shot[64];sprintf_s(shot,"custom-stage-%d-warp-%d.bmp",stage+1,warp);Screenshot(shot);
   std::vector<double> states;
   const auto capture=[&](CStage* st){
    const double v[]={double(st->m_curFrame),st->m_pChara->m_curX,st->m_pChara->m_curY,double(st->m_curScore),double(st->m_nCurLife),double(st->m_nCurBomb),double(st->m_pChara->m_power),double(st->m_curDream),double(st->m_playerPerformance),double(st->m_curNGraze),double(st->m_curNPointItem),double(st->m_totalNPointItem)};
    return std::vector<double>(v,v+12);
   };
   for(int frame=0;frame<360;frame++){
    unsigned short input=((frame/60)%2?4:8)|((frame/45)%2?64:0);
    record->StepFrame(input);const auto v=capture(record);states.insert(states.end(),v.begin(),v.end());
   }
   if(warp==1&&stage!=5)Check(record->m_pMidBoss!=NULL,"midboss teleport spawns the native midboss, not a replacement encounter");
   const int nextRandom=rand();
   record->SetQuit(STAGE_END_SAVE_EXIT);int end=0;for(int i=0;i<60&&!end;i++)end=record->StepFrame(0);
   Check(end==STAGE_END_SAVE_EXIT,"custom run closes the recording through the save action");
   Check(g.m_replay.SaveFile("custom-roundtrip.rpy"),"custom replay saves its versioned initial parameters");
   REPLAY_BRIEF_INFO info={};Check(CReplay::GetRepFileInfo(&info,"custom-roundtrip.rpy"),"custom replay is accepted by the native replay browser");
   delete record;game.m_pCurScheme=NULL;
   if((stage==0&&warp==3)||(stage==3&&warp==2)){
    CScheme* restarted=NULL;int id=0;
    Check(CSchemeSwitcher::SwitchScheme(&restarted,&id,4,STAGE_END_RESTART)&&id==4,"custom restart constructs a fresh real stage");
    CStage* st=static_cast<CStage*>(restarted);
    Check(g.m_practiceConfig.warp==warp&&st->m_nCurLife==4&&st->m_curNPointItem==123&&st->m_totalNPointItem==4567&&st->m_curScore==12345,"restart restores original custom destination and independent counters");
    g.m_replay.CloseStage(false);delete st;
   }
   Check(g.m_replay.LoadFile("custom-roundtrip.rpy"),"custom replay loads the complete parameter extension");
   Check(g.m_replay.m_practiceConfig.warp==warp&&g.m_replay.m_practiceConfig.rank==48&&g.m_replay.m_practiceConfig.graze==321,"replay metadata restores destination, rank lock and counters");
   g.m_bReplayMode=true;g.m_playStage=stage;g.OnBeginGame();
   CStage* playback=new CStage;playback->Initialize();game.m_pCurScheme=playback;Frame(*playback);
   bool equal=true;size_t offset=0;
   for(int frame=0;frame<360;frame++){
    playback->StepFrame(0);const auto v=capture(playback);for(auto n:v)if(n!=states[offset++])equal=false;
   }
   Check(equal,"custom replay matches all twelve gameplay state values on every recorded frame");
   Check(rand()==nextRandom,"custom recording and playback consume identical gameplay RNG sequences");
   g.m_replay.CloseStage(false);delete playback;game.m_pCurScheme=NULL;
  }
  g.m_bReplayMode=false;menu=practice::Config();menu.mode=1;menu.totalPoints=99;menu.points=7;menu.lives=3;menu.rank=48;menu.lockRank=1;
  Check(practice::StartSetup(),"independent blue-point counters initialize");CStage* counters=new CStage;counters->Initialize();game.m_pCurScheme=counters;
  counters->m_pChara->CaughtItem(0,0,ITEM_POINT);
  Check(counters->m_totalNPointItem==100&&counters->m_curNPointItem==8&&counters->m_nCurLife==5,"point pickup increments both counters and extends from cumulative count only");
  auto& chara=*counters->m_pChara;chara.m_invulnerableTimeLeft=0;chara.m_nextLifeComeTimeLeft=33;chara.m_cannotControlTimeLeft=1;chara.m_bMiss=false;chara.Step();
  Check(counters->m_playerPerformance==48,"locked rank survives the native Miss rank reset");
  g.m_replay.CloseStage(false);delete counters;game.m_pCurScheme=NULL;
  for(int bound=0;bound<2;bound++){
   menu=practice::Config();menu.mode=1;menu.lockRank=1;menu.lives=bound?8:0;menu.bombs=bound?8:0;menu.power=bound?128:0;menu.dream=bound?128:1;menu.rank=bound?64:0;menu.points=bound?999:0;menu.totalPoints=bound?99999:0;menu.score=bound?99999990:0;
   Check(practice::StartSetup(),"custom native resource boundaries can start");CStage* st=new CStage;st->Initialize();game.m_pCurScheme=st;
   Check(st->m_nCurLife==(bound?9:1)&&st->m_nCurBomb==(bound?8:0)&&st->m_pChara->m_power==(bound?128:1)&&st->m_curDream==(bound?128:1)&&st->m_playerPerformance==(bound?64:0),"configured lower and upper resource bounds match actual native state");
   Frame(*st);g.m_replay.CloseStage(false);delete st;game.m_pCurScheme=NULL;
  }
  // Hidden custom fields must not disable normal mode on Hard/Lunatic.
  menu.mode=0;menu.lockRank=0;menu.rank=0;g.m_initDifficulty=g.m_playDifficulty=2;g.m_practiceUnlocked[g.m_playChara][2]|=1;
  practice::BeginFrame();practice::DrawSetup(30);practice::EndFrame();
  Check(practice::StartSetup()&&!g.m_practiceConfig.mode,"normal mode can start an unlocked stage after switching from custom fields");
  g.m_initDifficulty=1;
  // Missing custom metadata must not silently play as a normal native replay.
  {FILE* fp=fopen("custom-roundtrip.rpy","rb");fseek(fp,0,SEEK_END);long size=ftell(fp);rewind(fp);std::vector<char> bytes(size);fread(bytes.data(),1,size,fp);fclose(fp);
   fp=fopen("custom-truncated.rpy","wb");fwrite(bytes.data(),1,size-1,fp);fclose(fp);CReplay invalid;
   Check(!invalid.LoadFile("custom-truncated.rpy"),"truncated custom parameters are rejected");
   REPLAY_BRIEF_INFO info={};Check(!CReplay::GetRepFileInfo(&info,"custom-truncated.rpy")&&info.infoRes==InfoRes_InvalidReplay,"browser rejects a custom replay missing required metadata");}
  Check(g.m_replay.LoadFile("practice-roundtrip.rpy")&&!g.m_replay.m_practiceConfig.mode,"legacy native Practice replay still loads without thprac metadata");
  g.m_bReplayMode=true;g.m_playStage=5;g.OnBeginGame();Check(!g.m_practiceConfig.mode&&g.m_playerPerformanceLowerBound!=48,"legacy replay does not inherit current custom rank locking");
  g.m_bReplayMode=false;g.m_bPracticeMode=false;g.m_playDifficulty=1;g.OnBeginGame();
  Check(!g.m_practiceConfig.mode&&!practice::Assisted(),"normal gameplay clears custom initial settings and assisted state");
 }
 static void AdvancedMenu(CGame& game){
  auto& opts=practice::Options();auto& g=CGame::GVar();opts=practice::AdvancedOptions();ResetTools();
  CStage* stage=Stage(game);auto& player=*stage->m_pChara;
  const int before=stage->m_curFrame;const size_t keys=g.m_replay.m_keyState[0].size();
  Key(game,VK_F12);Check(practice::AdvancedOpen(),"F12 opens advanced options");
  stage->StepFrame(32|8);Check(stage->m_curFrame==before&&g.m_replay.m_keyState[0].size()==keys,"F12 freezes gameplay without recording menu input");
  Frame(*stage);Screenshot("f12-gameplay.bmp");
  ImGuiWindow* window=ImGui::FindWindowByName("TH05 Advanced Options");
  Check(window&&window->Size.x==640&&window->Size.y==480&&(window->Flags&ImGuiWindowFlags_NoTitleBar),"advanced layout follows purple fullscreen borderless options");
  Key(game,VK_F12,true);Check(practice::AdvancedOpen(),"held F12 does not flicker the menu");
  stage->m_bPaused=true;practice::FocusPause();Key(game,'Z');Frame(*stage);Check(stage->m_bPaused,"F12 prevents hidden pause controls from resuming the game");stage->m_bPaused=false;
  const char* glyphs=practice::AdvancedGlyphs();int size=MultiByteToWideChar(CP_UTF8,0,glyphs,-1,NULL,0);std::vector<wchar_t> unicode(size);MultiByteToWideChar(CP_UTF8,0,glyphs,-1,unicode.data(),size);
  bool complete=true;for(auto ch:unicode)if(ch>=0x3000&&!ImGui::GetFont()->FindGlyphNoFallback(ch))complete=false;
  Check(complete,"all advanced labels have real glyphs instead of question marks");
  ImGuiWindow* child=window->DC.ChildWindows[0];
  const auto group=[&](const char* label,bool open){child->StateStorage.SetInt(child->GetID(label),open?1:0);};
  group(u8"\u6e38\u620f\u8fdb\u884c",false);group(u8"\u6e38\u620f\u901f\u5ea6",true);Frame(*stage);Screenshot("f12-speed.bmp");
  group(u8"\u6e38\u620f\u901f\u5ea6",false);group(u8"\u53cd\u5e94\u529b\u6d4b\u8bd5",true);
  practice::Reaction().Reset();srand(77);const int expectedRandom=rand();srand(77);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*stage);practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  Check(rand()==expectedRandom,"reaction test's private random generator does not consume gameplay RNG");
  practice::Reaction().Reset();practice::Reaction().Start(-10000,1,true);Frame(*stage);Screenshot("f12-reaction.bmp");
  group(u8"\u53cd\u5e94\u529b\u6d4b\u8bd5",false);group(u8"\u5173\u4e8ethprac",true);Frame(*stage);Screenshot("f12-about.bmp");
  Key(game,VK_F12);Check(!practice::AdvancedOpen(),"F12 closes options");
  opts.disableX=opts.disableShift=opts.disableZ=true;
  Check(practice::FilterGameplayInput(16|32|64|8,false)==8,"disabled X, Z and Shift are removed from game input");
  Check(practice::FilterGameplayInput(16|32|64|8,true)==(16|32|64|8),"recorded replay input bypasses current key-disable settings");
  opts.forceShift=true;Check(practice::FilterGameplayInput(8,false)==(8|64),"force low speed overrides disabled Shift like purple thprac");opts.forceShift=false;
  stage->StepFrame(16|32|64|8);Check(stage->m_curGameKeyState==8&&g.m_replay.m_keyState[0].back()==8,"live key filtering is saved in the actual native replay input stream");
  opts=practice::AdvancedOptions();opts.fps=112;stage->StepFrame(0);Check(game.m_fpsLimit==112,"configured game speed reaches the production frame limiter");opts.fps=56;
  Key(game,VK_F2);opts.noContinue=true;stage->m_nCurLife=3;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();
  Check(stage->m_nCurLife==2,"F2 no-continue mapping consumes spare lives normally");
  stage->m_nCurLife=1;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();Check(stage->m_nCurLife==1&&!stage->m_bContinueScr,"F2 no-continue retains the last life without Game Over");
  opts.noContinue=false;stage->m_nCurLife=3;player.m_nextLifeComeTimeLeft=1;player.m_cannotControlTimeLeft=1;player.Step();Check(stage->m_nCurLife==3,"unmapped F2 still locks all lives");ResetTools();
  player.m_curX=3072;player.m_curY=5120;player.m_nextLifeComeTimeLeft=0;player.m_invulnerableTimeLeft=0;player.m_collisionAreaAlpha=0;
  stage->m_pTextOverlay->SetMode(TEXT_OVERLAY_NONE);player.EnableDraw();
  std::vector<unsigned char> hidden(20*20*3),visible(20*20*3);const int x=stage->m_playAreaUpperLeftX+192-10,y=480-(stage->m_playAreaUpperLeftY+320)-10;
  Frame(*stage);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,hidden.data());
  opts.alwaysHitbox=true;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden!=visible,"always-show hitbox adds pixels while unfocused");
  opts.showKeys=true;Frame(*stage);Screenshot("f12-key-monitor-hitbox.bmp");Check(ImGui::FindWindowByName(u8"\u6309\u952e\u663e\u793a###TH05 Key Monitor")!=NULL,"key monitor uses the real stage input state");
  opts.showKeys=opts.alwaysHitbox=false;
  player.m_collisionAreaAlpha=100;stage->m_curGameKeyState=64;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden==visible,"default PC98 display hides hitbox even at low speed");
  opts.showHitbox=true;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden!=visible,"low-speed option shows hitbox during Shift");
  stage->m_curGameKeyState=0;Frame(*stage);glReadPixels(x,y,20,20,GL_RGB,GL_UNSIGNED_BYTE,visible.data());Check(hidden==visible,"low-speed option hides hitbox immediately after Shift release");
  practice::ResetRun();Check(practice::Assisted(),"hitbox cheat marks the run assisted before stage entry");opts.showHitbox=false;practice::ResetRun();
  CBossStage1* boss=new CBossStage1;boss->m_pStage=stage;boss->Initialize();boss->m_curPhase=4;stage->m_pBoss=boss;
  stage->TickLockTimer();stage->TickLockTimer();Check(stage->LockTimerFrames()==2,"phase timer counts native simulation updates");boss->m_curPhase=5;stage->TickLockTimer();Check(stage->LockTimerFrames()==1,"phase transition resets lock timing");
  stage->m_bPaused=true;const int timer=stage->LockTimerFrames();stage->StepFrame(0);Check(stage->LockTimerFrames()==timer,"paused game does not advance lock timing");stage->m_bPaused=false;
  stage->m_pBoss=NULL;delete boss;stage->TickLockTimer();Check(stage->LockTimerFrames()==0,"leaving boss battle clears lock timing");
  // Show the timer on a real resource-backed boss, then test automatic retry.
  g.m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;
  auto& menu=practice::MenuConfig();menu=practice::Config();menu.mode=1;menu.warp=2;menu.lives=3;
  Check(practice::StartSetup(),"advanced runtime test starts a real practice boss");stage=new CStage;stage->Initialize();game.m_pCurScheme=stage;
  opts.lockTimer=true;Key(game,VK_F5);stage->StepFrame(0);Frame(*stage);Screenshot("f12-lock-timer.bmp");ResetTools();opts.lockTimer=false;
  opts.fastRetry=true;stage->m_pChara->m_invulnerableTimeLeft=0;stage->m_pChara->m_bMiss=true;stage->m_pChara->Step();
  Check(stage->m_fastRetryCountdown==15&&stage->m_bPaused,"practice Miss opens the original fast-retry countdown");
  int end=0;for(int i=0;i<20&&!end;i++)end=stage->StepFrame(0);Check(end==STAGE_END_RESTART,"fast retry exits through the real restart route");
  CScheme* fresh=NULL;int id=0;Check(CSchemeSwitcher::SwitchScheme(&fresh,&id,4,end)&&id==4,"fast retry creates a fresh practice run");delete stage;stage=static_cast<CStage*>(fresh);game.m_pCurScheme=stage;
  Check(!stage->m_bLogo&&stage->m_nCurLife==4&&stage->m_pBoss!=NULL,"fast retry preserves practice initial values and skips the loading logo");
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_SHIFT,0);Key(game,'F');practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_SHIFT,1L<<31);stage->StepFrame(0);
  Check(stage->m_fastRetryCountdown==14,"Shift+F triggers the same practice retry path");
  g.m_replay.CloseStage(false);delete stage;game.m_pCurScheme=NULL;practice::TakeFastRetryRequest();opts=practice::AdvancedOptions();
  practice::ReactionTest test;test.Start(1000,1200,true);test.Tick(2000,true);Check(test.state==practice::ReactionTest::TooEarly&&test.results.empty(),"early press does not fabricate a reaction result");
  test.Start(1000,1200,true);test.Tick(2200,false);test.Tick(2250,true);Check(test.results.size()==1&&test.results[0]==50,"reaction latency is measured from the actual color-change deadline");
  test.Reset();test.type=1;test.Start(0,1500,true);test.Tick(100,true);test.Tick(1600,true);test.Tick(1640,false);Check(test.results.size()==1&&test.results[0]==40,"release reaction waits for hold then measures release latency");
  test.Reset();test.progress=true;test.Start(1000,1200,true);test.Tick(2150,true);Check(test.results.size()==1&&test.results[0]==-50&&test.frameCounts[0]==3,"progress mode preserves negative early-reaction timing");
  opts.fps=84;opts.noContinue=opts.disableX=opts.showKeys=opts.showHitbox=opts.alwaysHitbox=true;practice::SaveAdvancedOptions();opts=practice::AdvancedOptions();practice::LoadAdvancedOptions();
  Check(opts.fps==84&&opts.noContinue&&opts.disableX&&opts.showKeys&&opts.showHitbox&&opts.alwaysHitbox,"advanced preferences round-trip independently of game and replay saves");
  opts=practice::AdvancedOptions();practice::SaveAdvancedOptions();practice::SetAdvancedOpen(false);
 }
 static void MusicRoom(CGame& game){
  Check(CMusicRoom::Available(),"original Music Room resource package is available");
  CMusicRoom room;Check(room.Initialize(),"original MUSIC.PI, translated font, comments and archive load");
  Check(room.m_game==4&&room.m_selection==0&&room.m_playing==0,"Music Room opens at TH05 first track");
  for(int i=0;i<15;i++)room.Step();room.Draw();glFinish();Screenshot("music-room-th05.bmp");
  const int expected[]={14,18,24,28,23};int total=0;
  for(int g=0;g<5;g++){
   Check(musicTrackCounts[g]==expected[g],"original per-game track counts");room.m_game=g;
   for(int track=0;track<musicTrackCounts[g];track++){
    Check(!room.Line(g,track,0).empty(),"original fixed-record comment has a title");
    Check(room.Play(track),"actual PMDWin accepts and starts the original song");
    Check(room.m_playing==track&&!room.m_playFailed,"selected song is the playing song");++total;
   }
  }
  Check(total==107,"all 107 songs use the original order and play successfully");
  room.m_game=4;room.m_selection=0;room.m_top=0;room.Play(0);room.Input(0);room.Input(1);
  Check(room.m_selection==23&&room.m_top==12,"Up wraps to Quit and scrolls to last twelve rows");room.Input(0);room.Input(2);
  Check(room.m_selection==0&&room.m_top==0,"Down from Quit wraps to the first track");
  for(int i=0;i<13;i++){room.Input(0);room.Input(2);}Check(room.m_selection==13&&room.m_top==2,"track list scrolls past twelve rows");
  room.Input(0);room.Input(8);Check(room.m_game==0&&room.m_selection==0&&room.m_playing==0,"Right wraps TH05 to TH01 and starts its title theme");
  room.Input(0);room.Input(4);Check(room.m_game==4&&room.m_top==0,"Left wraps TH01 to TH05");
  room.m_selection=22;room.Input(0);room.Input(32);Check(room.m_playing==22,"Z starts the selected track");
  room.m_fade=100;room.m_commentAge=30;room.Draw();glFinish();Screenshot("music-room-comment.bmp");
  auto notes=CPMDPlayer::PianoNotes();for(auto note:notes)Check(note==255||(note>=0&&(note&15)<12&&(note>>4)<8),"piano notes come from valid live PMD work data");
  room.Input(0);room.Input(4096);Check(room.m_quit,"Esc exits Music Room");int ended=0;for(int i=0;i<20&&!ended;i++)ended=room.Step();Check(ended==1,"Music Room fades out before returning");
  CScheme* next=NULL;int id=0;Check(CSchemeSwitcher::SwitchScheme(&next,&id,8,1)&&id==2,"Music Room exit returns to native title");delete next;
 }
 static int Run() {
  DeleteFileA(".\\thprac-uth05.ini");
  CGame game; CGame::s_pCurGame=&game;
  Check(game.Initialize(false),"hidden production OpenGL/game initialization");
  Check(practice::Initialize(game.m_gameWindow.m_hWnd),"ImGui Win32/OpenGL2 initialization");
  CStage* s=Stage(game); CChara& p=*s->m_pChara;
  InactiveFloatingText(*s);
  for(int i=0;i<6;i++) { Key(game,VK_F1+i);Check(practice::Enabled(practice::Tool(i)),"F1-F6 enabled");Key(game,VK_F1+i,true);Check(practice::Enabled(practice::Tool(i)),"key repeat does not toggle"); }
  Key(game,'U');Check(practice::Enabled(practice::EnemyInvincible),"U enabled");Key(game,VK_F7);Check(practice::Enabled(practice::EnemyInvincible),"F7 does not modify tools");ResetTools();
  Key(game,VK_F1);p.m_bMiss=true;const int life=s->m_nCurLife,power=p.m_power,bombs=s->m_nCurBomb;p.Step();
  Check(!p.m_bMiss&&p.m_nextLifeComeTimeLeft==0&&s->m_nCurLife==life&&p.m_power==power&&s->m_nCurBomb==bombs&&s->m_nMiss==0,"F1 collision leaves resources/state intact");Check(s->m_bPlaySound[2],"F1 collision miss sound");ResetTools();p.m_invulnerableTimeLeft=0;
  Key(game,VK_F3);s->m_nCurBomb=0;p.DropBomb();Check(s->m_bInCharaBomb&&s->m_nCurBomb==0,"F3 zero-stock Bomb does not underflow");ResetTools();s->m_bInCharaBomb=false;s->m_nCurBomb=2;p.m_invulnerableTimeLeft=0;
  Key(game,VK_F6);p.m_bMiss=true;p.Step();Check(s->m_bInCharaBomb&&s->m_nCurBomb==1&&p.m_nextLifeComeTimeLeft==0&&s->m_nMiss==0,"F6 collision uses native Bomb before death");ResetTools();s->m_bInCharaBomb=false;
  p.m_invulnerableTimeLeft=0;p.m_nextLifeComeTimeLeft=33;p.m_cannotControlTimeLeft=1;p.m_bMiss=false;p.SetPower(100);s->m_nCurLife=1;
  Key(game,VK_F2);Key(game,VK_F4);p.Step();Check(p.m_power==100,"F4 preserves Miss firepower");
  p.m_nextLifeComeTimeLeft=1;p.Step();Check(s->m_nCurLife==1&&!s->m_bContinueScr,"F2 last life revives without game over");ResetTools();
  Check(practice::AllowTimeout(true)&&!practice::AllowTimeout(false),"normal deadline gate");Key(game,VK_F5);Check(!practice::AllowTimeout(true)&&practice::BeforeTimeout(false),"F5 blocks timeout while retaining hit path");
  // Original Phase4 runs its attack and tests damage after the time cap.
  CBossStage1 boss;boss.m_pStage=s;boss.Initialize();boss.m_curPhase=4;boss.m_curPhaseFrame=1299;boss.m_curHP=400;boss.m_curPhaseEndHP=0;
  s->m_pChara->m_invulnerableTimeLeft=10000;
  boss.Step();Check(boss.m_curPhase==4&&boss.m_curPhaseFrame==1300,"F5 keeps Boss phase advancing past deadline");
  s->m_pCharaBullet->AddBullet(boss.m_curX,boss.m_curY,3,0,0,0,0,255);
  s->m_pCharaBullet->AddBullet(boss.m_curX,boss.m_curY,3,0,0,0,0,255);
  boss.Step();Check(boss.m_curPhase==0xfd&&boss.m_bSuccessDefeat,"F5 still permits defeat after expired deadline");
  s->m_pCharaBullet->Initialize();
  CMidBossStage1 mid;mid.m_pStage=s;mid.Initialize();mid.m_curPhase=3;mid.m_curPhaseFrame=1499;
  mid.Step();Check(mid.m_curPhase==3&&mid.m_curPhaseFrame==1500,"F5 keeps MidBoss alive beyond deadline");
  const int rewardBefore=s->m_curScore;mid.m_curHP=100;
  for(int i=0;i<10;i++)s->m_pCharaBullet->AddBullet(mid.m_curX,mid.m_curY,3,0,0,0,0,255);
  mid.Step();Check(mid.m_curPhase==0xfe&&s->m_curScore>rewardBefore,"expired locked MidBoss still grants defeat reward");
  ResetTools();s->m_pCharaBullet->Initialize();boss.Initialize();boss.m_curPhase=4;boss.m_curPhaseFrame=1299;boss.m_curHP=400;
  boss.Step();Check(boss.m_curPhase==0xfd&&!boss.m_bSuccessDefeat,"disabling F5 restores original timeout");
  Key(game,'U');s->m_pCharaBullet->AddBullet(1000,1000,3,0,0,0,0,80);Check(s->GetDamageFromChara(1000,1000,300,300,true)==0,"U suppresses actual projectile damage");ResetTools();
  CGame::GVar().m_bReplayMode=true;for(int i=0;i<6;i++)Key(game,VK_F1+i);Key(game,'U');
  for(int i=0;i<practice::ToolCount;i++)Check(!practice::Enabled(practice::Tool(i)),"Replay isolates live tools");
  CGame::GVar().m_bReplayMode=false;for(int i=0;i<practice::ToolCount;i++)Check(!practice::Enabled(practice::Tool(i)),"replay hotkeys do not arm future cheats");ResetTools();
  Check(s->Pause(),"Esc pause entry");for(int i=0;i<12;i++)Frame(*s);Screenshot();
  Check(s->m_bPaused,"upstream pause drawing does not advance gameplay");
  ImGuiWindow* pause=ImGui::FindWindowByName("Pause Menu");
  Check(pause&&pause->Pos.x==32&&pause->Pos.y==16&&pause->Size.x==384&&pause->Size.y==448,"THPauseMenu position and size match purple source");
  Check(pause&&(pause->Flags&ImGuiWindowFlags_NoTitleBar),"upstream pause has no invented title bar");
  const ImVec2 button(220,176);
  ImGui::GetIO().MousePos=button;ImGui::GetIO().MouseDown[0]=true;
  ImGui::NewFrame();practice::DrawPause(*s);ImGui::Render();
  ImGui::GetIO().MousePos=button;ImGui::GetIO().MouseDown[0]=false;
  ImGui::NewFrame();practice::DrawPause(*s);ImGui::Render();
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"upstream Resume click closes pause after fade");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"Z confirms focused Resume after mouse use");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_ESCAPE,0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_ESCAPE,1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(!s->m_bPaused,"Esc resumes through the upstream fade");
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,VK_DOWN,0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,VK_DOWN,1L<<31);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Z',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Z',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_SAVE_EXIT,"Down and Z activate upstream Exit with replay-save path");
  s->m_bQuit=false;s->m_bCanPause=true;
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'Q',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'Q',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_PLAYER_QUIT,"Q uses Exit without replay saving");
  s->m_bQuit=false;s->m_bCanPause=true;
  Key(game,VK_BACK);Frame(*s);Frame(*s);Screenshot("overlay.bmp");
  ImGuiWindow* overlay=ImGui::FindWindowByName("Mod Menu");
  Check(overlay&&overlay->Pos.x==10&&overlay->Pos.y==10&&(overlay->Flags&ImGuiWindowFlags_NoNav),"THOverlay position and flags match purple source");
  ImGui::GetIO().MousePos=ImVec2(35,24);ImGui::GetIO().MouseDown[0]=true;
  ImGui::NewFrame();practice::DrawStatus();ImGui::Render();
  ImGui::GetIO().MousePos=ImVec2(35,24);ImGui::GetIO().MouseDown[0]=false;
  ImGui::NewFrame();practice::DrawStatus();ImGui::Render();
  Check(practice::Enabled(practice::Invincible),"upstream clickable text shares F1 state");
  Frame(*s);Screenshot("overlay-enabled.bmp");
  Key(game,VK_F1);Key(game,VK_BACK);
  s->Pause();for(int i=0;i<12;i++)Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYDOWN,'R',0);Frame(*s);
  practice::WindowMessage(game.m_gameWindow.m_hWnd,WM_KEYUP,'R',1L<<31);
  for(int i=0;i<10;i++)Frame(*s);
  Check(s->m_bQuit&&s->m_quitCode==STAGE_END_RESTART&&!s->m_bPaused,"R requests Restart through fade");
  CGame::GVar().m_playStage=3;
  CScheme* next=NULL;int nextID=0;
  Check(CSchemeSwitcher::SwitchScheme(&next,&nextID,4,STAGE_END_RESTART)&&nextID==4&&CGame::GVar().m_playStage==0,"Restart initializes a fresh normal run");
  delete next;
  CGame::GVar().m_bCanSaveReplay=true;practice::MarkAssisted();Check(CGame::GVar().m_bCanSaveReplay,"live tools preserve the replay-save option");
  delete s;game.m_pCurScheme=NULL;NativePractice(game);CustomPractice(game);MusicRoom(game);AdvancedMenu(game);game.OnGameEnd();printf("PASS %d production practice checks\n",checks);return 0;
 }
};int PracticeTests::checks=0;
}
int main() {return th5w::PracticeTests::Run();}

