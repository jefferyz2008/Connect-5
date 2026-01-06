#include <iostream>
using namespace std;
//1 is now X, 2 is now O
#include <vector>
#include <array>

#include <limits>
#include <algorithm>
using namespace std;
#include <Servo.h>

bool hasStreak(const std::vector<int>& line, int value)
{
    int count=0;
    for (int x:line)
    {
        if (x==value)
        {
            count+=1;
            if (count>=5)
            {
                return true;
            }
            
        }
        else
        {
            count=0;
        }
    }
    return false;
}

std::array<std::array<int, 8>, 8> invert(std::array<std::array<int, 8>, 8> grid)
{
    std::array<std::array<int, 8>, 8> inverted = {}; // initialize all to 0

    for (size_t r = 0; r < 8; ++r) {
        for (size_t c = 0; c < 8; ++c) {
            inverted[c][r] = grid[r][c]; // swap rows and columns
        }
    }

    return inverted;
}


bool isMoveLegal(const std::array<std::array<int, 8>, 8> grid, int col)
{
    for (const std::array<int, 8> row:grid)
    {
       if (row[col]==0)
       {
        return true;
       }
    }
    return false;
}

int scoreLine(const std::vector<int> line,int color)
{
    int opp = (color == 1) ? 2 : 1;
    int score=0;
    for (int i=0; i<line.size()-4; i++)
    {
        std::array<int, 5> window;
        for (int j = 0; j < 5; ++j) 
        {
             window[j] = line[i + j];
        }
        if (std::find(window.begin(), window.end(), opp) != window.end()) 
        {
          continue; // blocked
        }
        int count = std::count(window.begin(), window.end(), color);

        if (count==5)
        {
            score+=100000;
        }
        else if (count==4)
        {
            score+=10000;
        }
        else if (count==3)
        {
            score+=1000;
        }
        else if (count==2)
        {
            score+=100;
        }
        else
        {
            score+=10;
        }
     
    }
    return score;
}

int fastHeuristic(const std::array<std::array<int, 8>, 8> grid, int color) {
    int score = 0;
    int opp = (color == 1) ? 2 : 1;

    // Helper to score a 5-slot window without using vectors
    auto evaluateWindow = [&](int countMine, int countOpp) {
        if (countOpp > 0) return 0; // Blocked
        if (countMine == 5) return 100000;
        if (countMine == 4) return 10000;
        if (countMine == 3) return 1000;
        if (countMine == 2) return 100;
        return 10;
    };

    // 1. Horizontal Scan
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c <= 3; c++) {
            int mine = 0, theirs = 0;
            for (int i = 0; i < 5; i++) {
                if (grid[r][c + i] == color) mine++;
                else if (grid[r][c + i] == opp) theirs++;
            }
            score += evaluateWindow(mine, theirs);
        }
    }

    // 2. Vertical Scan
    for (int c = 0; c < 8; c++) {
        for (int r = 0; r <= 3; r++) {
            int mine = 0, theirs = 0;
            for (int i = 0; i < 5; i++) {
                if (grid[r + i][c] == color) mine++;
                else if (grid[r + i][c] == opp) theirs++;
            }
            score += evaluateWindow(mine, theirs);
        }
    }

    // 3. Diagonal Scan (Down-Right)
    for (int r = 0; r <= 3; r++) {
        for (int c = 0; c <= 3; c++) {
            int mine = 0, theirs = 0;
            for (int i = 0; i < 5; i++) {
                if (grid[r + i][c + i] == color) mine++;
                else if (grid[r + i][c + i] == opp) theirs++;
            }
            score += evaluateWindow(mine, theirs);
        }
    }

    // 4. Diagonal Scan (Up-Right)
    for (int r = 4; r < 8; r++) {
        for (int c = 0; c <= 3; c++) {
            int mine = 0, theirs = 0;
            for (int i = 0; i < 5; i++) {
                if (grid[r - i][c + i] == color) mine++;
                else if (grid[r - i][c + i] == opp) theirs++;
            }
            score += evaluateWindow(mine, theirs);
        }
    }

    return score;
}
bool checkWinner(const std::array<std::array<int, 8>, 8>& grid, int color) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (c <= 3 && grid[r][c] == color && grid[r][c+1] == color && grid[r][c+2] == color && grid[r][c+3] == color && grid[r][c+4] == color) return true;
            if (r <= 3 && grid[r][c] == color && grid[r+1][c] == color && grid[r+2][c] == color && grid[r+3][c] == color && grid[r+4][c] == color) return true;
            if (r <= 3 && c <= 3 && grid[r][c] == color && grid[r+1][c+1] == color && grid[r+2][c+2] == color && grid[r+3][c+3] == color && grid[r+4][c+4] == color) return true;
            if (r >= 4 && c <= 3 && grid[r][c] == color && grid[r-1][c+1] == color && grid[r-2][c+2] == color && grid[r-3][c+3] == color && grid[r-4][c+4] == color) return true;
        }
    }
    return false;
}


void placePiece(std::array<std::array<int, 8>, 8> &grid,int col, int color)
{
    for (int i=7; i>-1; i--)
    {
        if (grid[i][col]==0)
        {
            grid[i][col]=color;
            break;
        }
    }
}

void removePiece(std::array<std::array<int, 8>, 8> &grid,int col)
{
    for (int i=0; i<8; i++)
    {
        if (grid[i][col]!=0)
        {
            grid[i][col]=0;
            break;
        }
    }
}

std::vector<int> allLegalMoves(const std::array<std::array<int, 8>, 8> &grid)
{
    std::vector<int> allLegal={};
    for (int col=0; col<8; col++)
    {
       if (isMoveLegal(grid,col))
       {
           allLegal.push_back(col);
       }
    }
    return allLegal;
}

long long miniMax(std::array<std::array<int, 8>, 8> &grid,int depth, long long alpha, 
                                              long long beta, bool maximizer)
{
    bool xWon=checkWinner(grid,1);
    bool oWon=checkWinner(grid,2);
    if (depth==0 || xWon || oWon)
    {
        if (xWon)
        {
            return 100000000000LL;
        }
        if (oWon)
        {
            return -100000000000LL;
        }
        return fastHeuristic(grid, 1) - fastHeuristic(grid, 2);
    }
    std::vector<int> allMoves=allLegalMoves(grid);
    if (maximizer)
    {
        long long maxScore = -10000000000000LL;
        for (int col:allMoves)
        {
            placePiece(grid, col, 1);
            long long score = miniMax(grid, depth-1, alpha, beta, false);
            removePiece(grid,col);
            alpha = max(alpha, score);
            if (beta <= alpha)
                break;
            maxScore = max(score, maxScore);
        }
        return maxScore;
    }
    else 
    {
        long long minScore = 10000000000000LL;
        for (int col:allMoves)
        {
            placePiece(grid, col, 2);
            long long score = miniMax(grid, depth-1, alpha, beta, true);
            removePiece(grid,col);
            beta = min(beta, score);
            if (beta <= alpha)
                break;
             minScore = min(score, minScore);
        }
        return minScore;
    }

}

int getBestMove(std::array<std::array<int, 8>, 8> grid,int color)
{
  
    int bestCol=0;
    std::vector<int> allMoves=allLegalMoves(grid);
    if (color==1)
    {
        for (int col2:allMoves)
            {
                placePiece(grid,col2,2);
                if (checkWinner(grid,2))
                {


                   removePiece(grid,col2);
                   return col2;
                }
                removePiece(grid,col2);

            }

        long long maxScore = -10000000000000LL;
        for (int col:allMoves)
        {

            placePiece(grid,col,1);
            std::vector<int> allMoves2=allLegalMoves(grid);

     
       
            if (checkWinner(grid, 1))
            {

                removePiece(grid,col);
                return col;
            }
            long long score=miniMax(grid,7,-10000000000000LL,10000000000000LL,false);
            if (score>maxScore)
            {
                bestCol=col;
                maxScore=score;
            }
            removePiece(grid,col);
        }
        return bestCol;

    }
    else // minimizing player (other color)
    {
        for (int col2:allMoves)
            {
                placePiece(grid,col2,1);
                if (checkWinner(grid,1))
                {
                   removePiece(grid,col2);
                   return col2;

                }
                removePiece(grid,col2);
            
            }
            
        long long minScore = 10000000000000LL;

        for (int col : allMoves)
        {
  
            placePiece(grid, col, 2); // assuming 2 is the other color
            std::vector<int> allMoves2=allLegalMoves(grid);



            if (checkWinner(grid, 2))

            {
               
                removePiece(grid,col);
                return col;
            }


            long long score = miniMax(grid, 7, -10000000000000LL, 10000000000000LL, true);
            if (score < minScore)
            {
                bestCol = col;
                minScore = score;
            }
            removePiece(grid,col);
        }
    }

    return bestCol;
}
  
  
std::array<std::array<int,8>,8> grid = {}; // empty board
int playerColor = 1; // human = X
int aiColor = 2;

Servo servos[8];                     
const int servoPins[8] = {37,35,33,31,29,27,25,23};

const int beamPins[8] = {22,24,26,28,30,32,34,36};

bool winnerFound=false;

int turn=1;


std::array<bool, 8> readBreakbeams() {
    std::array<bool, 8> beamStates;

    for (int i = 0; i < 8; i++) {
        // Assuming LOW = broken, HIGH = unbroken
        beamStates[i] = (digitalRead(beamPins[i]) == LOW);
    }

    return beamStates;
}

int differentValue(const std::array<bool, 8>& a, const std::array<bool, 8>& b) {
    for (int i = 0; i < 8; i++) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return 67;
}

void moveServo(int servoNum)
{
    bool initialBeam;
    bool currentBeam;
    int time=0;
    bool shouldBreak=false;
    Servo &servo=servos[servoNum];
    initialBeam=(digitalRead(beamPins[servoNum]) == LOW);
    int times[8]={50,50,50,50,50,50,50,50};
    int angles[8]={59,57,59,59,59,55,52,59};
    int numMoves=0;
    while (true)
    {
       numMoves+=1;
       if (numMoves>=7)
       {
        times[servoNum]+=5;
       }

       time=0;
       servo.write(angles[servoNum]);
       while (time<times[servoNum])
       {
           delay(1);
           time+=1;
           currentBeam=(digitalRead(beamPins[servoNum]) == LOW);
           if (currentBeam)
           {
            servo.write(12);
            shouldBreak=true;
            break;
           }
       }
       servo.write(12);
       time=0;
       while (time<500 && !shouldBreak)
       {
           delay(1);
           time+=1;
           currentBeam=(digitalRead(beamPins[servoNum]) == LOW);
           if (currentBeam)
           {
            shouldBreak=true;
            break;
           }
       }
       if (shouldBreak)
       {
        servo.write(12);
        break;
       }

    }
}

void checkStuckPieces(int servoNum)
{
    bool initialBeam;
    bool currentBeam;
    int time=0;
    bool shouldBreak=false;
    Servo &servo=servos[servoNum];
    initialBeam=(digitalRead(beamPins[servoNum]) == LOW);
    if (!initialBeam)
    {
        turn=1;
        return;
    }
    int times[8]={65,65,65,65,65,65,65,65};
    int angles[8]={55,55,55,58,55,55,53,55};

    while (true)
    {
       time=0;
       servo.write(angles[servoNum]);
       while (time<times[servoNum])
       {
           delay(1);
           time+=1;
           currentBeam=(digitalRead(beamPins[servoNum]) == LOW);
           if (!currentBeam)
           {
            servo.write(12);
            shouldBreak=true;
            break;
           }
       }
       servo.write(12);
       time=0;
       while (time<500 && !shouldBreak)
       {
           delay(1);
           time+=1;
           currentBeam=(digitalRead(beamPins[servoNum]) == LOW);
           if (!currentBeam)
           {
            shouldBreak=true;
            break;
           }
       }
       if (shouldBreak)
       {servo.write(12);
        break;
       }

    }
    turn=1;
}


void setup() {
    randomSeed(analogRead(A0));
   Serial.begin(9600);
   pinMode(44, OUTPUT);
   pinMode(44, OUTPUT);
   pinMode(45, OUTPUT);
   pinMode(47, OUTPUT);

   //45 46 yellow
   //44 47 red

  // Attach servos
  for (int i = 0; i < 8; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(12);
  }

  // IR breakbeams
  for (int i = 0; i < 8; i++) {
    pinMode(beamPins[i], INPUT_PULLUP);
  }

}



void loop2() {
    for (int i = 0; i < 8; i++) {
        moveServo(i);
        delay(2000);
        checkStuckPieces(i);
        delay(200); // small pause before next servo
        digitalWrite(45,HIGH);
        digitalWrite(46,HIGH);
        delay(500);
        digitalWrite(45,LOW);
        digitalWrite(46,LOW);
        delay(500);
    }
}


void loop() {
  if (checkWinner(grid,1))
  {
    while (true)
    {
        digitalWrite(45,HIGH);
        digitalWrite(46,HIGH);
        delay(500);
        digitalWrite(45,LOW);
        digitalWrite(46,LOW);
        delay(500);
    }
  }
  if (checkWinner(grid,2))
  {
    while (true)
    {
        digitalWrite(44,HIGH);
        digitalWrite(47,HIGH);
        delay(500);
        digitalWrite(44,LOW);
        digitalWrite(47,LOW);
        delay(500);
    }
  }
  if (turn==1)
  {
    //turn on led 
    digitalWrite(47,LOW);
    digitalWrite(44,LOW);
    digitalWrite(45,HIGH);
    digitalWrite(46,HIGH);
    //if human turn
    std::array<bool, 8> initialBeams=readBreakbeams();
    while (true)
    {
      std::array<bool, 8> currentBeams=readBreakbeams();
      int value=differentValue(initialBeams,currentBeams);
      if (value!=67)
      {
        placePiece(grid,value,1);
        turn=2;
        break;
      }
    }
  }
  else
  {
    digitalWrite(47,HIGH);
    digitalWrite(44,HIGH);
    digitalWrite(45,LOW);
    digitalWrite(46,LOW);
    int bestMove=getBestMove(grid,2);
    delay(2000);
    placePiece(grid,bestMove,2);
    moveServo(bestMove);
    delay(2000);
    checkStuckPieces(bestMove);
  }
}
