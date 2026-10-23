#pragma once
namespace th5w { namespace practice {
// Read-only projection of the native timeout condition. Negative means no timeout.
struct PhaseTimeout {
 enum Unit { None, Seconds, Turns };
 Unit unit; int value;
 PhaseTimeout():unit(None),value(0){}
 PhaseTimeout(Unit u,int n):unit(u),value(n>0?n:0){}
 static PhaseTimeout Frames(int deadline,int elapsed){int n=deadline-elapsed;return PhaseTimeout(Seconds,n>0?(n+55)/56:0);}
 static PhaseTimeout Cycles(int limit,int completed){return PhaseTimeout(Turns,limit-completed);}
};
} }
