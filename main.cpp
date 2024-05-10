//
// Created by Cruz Castro, Laura M on 4/10/24.
//
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include<fstream>
#include<vector>
#include<chrono>
#include<sstream>
#include<algorithm>
#include<iomanip>
using namespace std;
//variables



//function
void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left+textRect.width/2.0f, textRect.top+textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x,y));
}





//class of tiles;
class Tile{
public:
    bool isMine; bool isRevealed; bool isFlagged;
    int adjacentMines;
    sf::Sprite spriteTile;
    Tile(): isMine(false), isRevealed(false), isFlagged(false), adjacentMines(0) {}
//reveal the game
    void reveal(vector<vector<Tile>>& board, int x, int y, int rowCount, int colCount, sf::Texture& hidden, sf::Texture& revealed, sf::Texture* numbers, sf::Texture& mine, sf::Texture& flag, bool& gameOver, bool debugMode) {
        if (isRevealed || isFlagged || (gameOver && !debugMode)) return;

        isRevealed = true;
        if (isMine && !debugMode) {
            gameOver = true; // 如果不是debug模式，设置游戏结束标志
            // 显示所有地雷
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    if (board[i][j].isMine) {
                        board[i][j].isRevealed = true;
                        board[i][j].spriteTile.setTexture(mine);
                    }
                }
            }
            return;
        }

        updateTexture(hidden, revealed, numbers, mine, flag);

        // 递归展开周围区域
        if (adjacentMines == 0) {
            for (int i = max(0, x-1); i <= min(x+1, rowCount-1); i++) {
                for (int j = max(0, y-1); j <= min(y+1, colCount-1); j++) {
                    if (i != x || j != y) board[i][j].reveal(board, i, j, rowCount, colCount, hidden, revealed, numbers, mine, flag, gameOver, debugMode);
                }
            }
        }
    }
    void updateTexture(sf::Texture& hidden, sf::Texture& revealed, sf::Texture* numbers, sf::Texture& mine, sf::Texture& flag) {

        if (isRevealed) {
            if (isMine) {
                spriteTile.setTexture(mine);
            } else if (adjacentMines > 0) {
                spriteTile.setTexture(numbers[adjacentMines - 1]);
            } else {
                spriteTile.setTexture(revealed);
            }
        } else {
            if (isFlagged) {
                spriteTile.setTexture(flag);

            } else {
                spriteTile.setTexture(hidden);
            }
        }
    }
};
bool checkWin(const vector<vector<Tile>>& board) {
    for (const auto& row : board) {
        for (const auto& tile : row) {
            if ((!tile.isMine && !tile.isRevealed)) {
                return false;
            }
        }
    }

    return true;
}
//restart the game

//timer
// 函数用于更新计时器的显示

vector<sf::Text> nametranslator(vector<string> input, sf::Font& font, int col, int row) {
    vector<sf::Text> display;
    for(int i = 0; i < 5; i++) {
        sf::Text text;  // 创建一个新的sf::Text对象
        text.setFont(font);  // 设置字体
        text.setCharacterSize(18);  // 设置字符大小
        text.setFillColor(sf::Color::White);  // 设置字体颜色为白色
        text.setStyle(sf::Text::Bold);  // 设置字体样式为粗体
        text.setString(input[i]);  // 设置显示的字符串
        setText(text, col * 16 / 2, (row * 16 + 50) / 2 +20);  // 设置文本位置
        display.push_back(text);  // 将设置好的text对象加入到vector中
    }

    for(int i=0; i<5; i++){
        cout<<display[i].getString().toAnsiString();
    }
    return display;
}


#include <chrono>

class Timer {
public:
    Timer() : _isRunning(false), _isPaused(false) {}

    void start() {
        _startPoint = std::chrono::steady_clock::now();
        _isRunning = true;
        _isPaused = false;
    }

    void pause() {
        if (_isRunning && !_isPaused) {
            _pausePoint = std::chrono::steady_clock::now();
            _isPaused = true;
        }
    }

    void resume() {
        if (_isRunning && _isPaused) {
            _startPoint += std::chrono::steady_clock::now() - _pausePoint;
            _isPaused = false;
        }
    }

    void reset() {
        _isRunning = false;
        _isPaused = false;
    }

    int getElapsedTime() const {
        if (_isRunning) {
            auto endPoint = _isPaused ? _pausePoint : std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::seconds>(endPoint - _startPoint).count();
        }
        return 0;
    }

private:
    std::chrono::steady_clock::time_point _startPoint;
    std::chrono::steady_clock::time_point _pausePoint;
    bool _isRunning;
    bool _isPaused;
};


void updateTimerDisplay(const Timer& timer, sf::Sprite* digitSprites, sf::RenderWindow& window, int col, int row) {
    int elapsedTime = timer.getElapsedTime(); // 获取已过时间（秒）
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;

    int minTens = minutes / 10;
    int minOnes = minutes % 10;
    int secTens = seconds / 10;
    int secOnes = seconds % 10;

    digitSprites[0].setTextureRect(sf::IntRect(minTens * 21, 0, 21, 32)); // 分钟的十位
    digitSprites[1].setTextureRect(sf::IntRect(minOnes * 21, 0, 21, 32)); // 分钟的个位
    digitSprites[2].setTextureRect(sf::IntRect(secTens * 21, 0, 21, 32)); // 秒的十位
    digitSprites[3].setTextureRect(sf::IntRect(secOnes * 21, 0, 21, 32)); // 秒的个位

    // 根据提供的图片信息调整基准位置
    float baseXForMinutes = (col * 32) - 97; // 分钟数的基准X位置
    float baseXForSeconds = (col * 32) - 54; // 秒数的基准X位置
    float baseY = (row + 0.5f) * 32 + 16;    // Y位置不变

    // 绘制分钟
    digitSprites[0].setPosition(baseXForMinutes, baseY);
    window.draw(digitSprites[0]);
    digitSprites[1].setPosition(baseXForMinutes + 21, baseY);
    window.draw(digitSprites[1]);

    // 绘制秒
    digitSprites[2].setPosition(baseXForSeconds, baseY);
    window.draw(digitSprites[2]);
    digitSprites[3].setPosition(baseXForSeconds + 21, baseY);
    window.draw(digitSprites[3]);
}

std::string getTimerString(const Timer& timer) {
    int elapsedTime = timer.getElapsedTime(); // 获取已过时间（秒）
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}


void updateCounterDisplay(int mineCount, int flagsCount, sf::RenderWindow &window, sf::Sprite *digitSprites, int row) {
    int remainingMines = mineCount - flagsCount;

    // 计算百位、十位和个位数字

    int hundreds = abs(remainingMines) / 100;
    int tens = (abs(remainingMines) / 10) % 10;
    int ones = abs(remainingMines) % 10;

    // 为每个位设置纹理矩形并更新位置
    digitSprites[0].setTextureRect(sf::IntRect(hundreds * 21, 0, 21, 32));
    digitSprites[1].setTextureRect(sf::IntRect(tens * 21, 0, 21, 32));
    digitSprites[2].setTextureRect(sf::IntRect(ones * 21, 0, 21, 32));

    // 如果地雷计数器为负，则添加负号的纹理矩形
    if (remainingMines < 0) {
        digitSprites[3].setTextureRect(sf::IntRect(10 * 21, 0, 21, 32)); // 负号
    } else {
        digitSprites[3].setTextureRect(sf::IntRect(0, 0, 0, 0)); // 无符号
    }

    // 计算基准位置并绘制每个数字
    float baseX = 33, baseY = 32 * ( row+ 0.5f) + 16;
    for (int i = 0; i < 4; ++i) {
        if (i == 3 && remainingMines >= 0) continue; // 如果没有负号，则跳过
        digitSprites[i].setPosition(baseX + i * 21, baseY); // 设置位置
        window.draw(digitSprites[i]); // 绘制到窗口
    }
}


void putmine(int rowCount, int colCount, int minecount, vector<vector<Tile>>& board){
    int placedMine=0;
    while(placedMine<minecount){
        int row=rand() % rowCount;
        int col=rand() % colCount;
        if(!board[row][col].isMine){
            board[row][col].isMine=true;
            placedMine++;}}
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            int count = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < rowCount && nj >= 0 && nj < colCount && board[ni][nj].isMine) {
                        count++;
                    }
                }
            }
            board[i][j].adjacentMines = count;
        }
    }
}
// 重新设置游戏的状态
void resetGame(vector<vector<Tile>>& board, int rowCount, int colCount, int mineCount, sf::Texture& hiddenTileTexture, bool& gameOver, bool& gameWon, int& flagCount) {
    // 清除旗子数量
    flagCount = 0;

    // 重置每个格子的状态
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            board[i][j].isMine = false;
            board[i][j].isRevealed = false;
            board[i][j].isFlagged = false;
            board[i][j].adjacentMines = 0;
            board[i][j].spriteTile.setTexture(hiddenTileTexture);
        }
    }

    // 随机放置地雷
    putmine(rowCount, colCount, mineCount, board);

    // 游戏状态重置
    gameOver = false;
    gameWon = false;
}



// 用于显示排行榜的函数
void showLeaderboard(sf::RenderWindow& window, bool& isPaused, bool& leaderboardActive, int col, int row, bool wasPausedBeforeLeaderboard, bool gameover) {
    // 暂停计时器
    wasPausedBeforeLeaderboard = isPaused;

    isPaused = true;

    // 创建并显示排行榜窗口
    sf::RenderWindow leaderboardWindow(sf::VideoMode(row*16+50, col*16), "Leaderboard", sf::Style::Titlebar | sf::Style::Close);
    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
            }

            if(!gameover)
                leaderboardWindow.clear(sf::Color::Black);
            // 这里添加绘制排行榜内容的代码
            leaderboardWindow.display();

        }


    }

    // 排行榜关闭后，恢复之前的暂停状态
    isPaused = wasPausedBeforeLeaderboard;
    leaderboardActive = false;
}


vector<string> nameranking(string name = "", string time = "", string file = "files/leaderboard.txt") {
    ifstream inputfile(file);
    string line;
    vector<string> ranking;

    // 读取现有排名
    while (getline(inputfile, line)) {
        ranking.push_back(line);
    }
    inputfile.close(); // 关闭输入文件

    // 如果有新的名字加入
    if (!name.empty()) {
        string new_entry = time + ", " + name;
        ranking.push_back(new_entry); // 添加新的记录
    }
    // 写回文件
    ofstream outputfile(file);
    for (const auto& entry : ranking) {
        outputfile << entry << endl; // 将更新后的排名重新写入文件
    }
    // 排序并截断排名到前五名
    sort(ranking.begin(), ranking.end());
    if (ranking.size() > 5) {
        ranking.resize(5);
    }



    return ranking;
}

std::vector<std::string> formatRankingsForDisplay(const std::vector<std::string>& rankings) {
    std::vector<std::string> formattedRankings;
    int num=0;
    for (const auto& record : rankings) {
        std::size_t commaPos = record.find(',');
        std::string time = record.substr(0, commaPos);
        std::string name = record.substr(commaPos + 2); // 跳过逗号和空格
        num=num+1;
        std::string str = std::to_string(num);
        formattedRankings.push_back(str+"."+"\t"+time + "\t" + name);
    }

    // 确保最后一个条目不会有额外的换行符
    std::string combinedRankings;
    for (size_t i = 0; i < formattedRankings.size(); ++i) {
        combinedRankings += formattedRankings[i];
        if (i < formattedRankings.size() - 1) {
            combinedRankings += "\n\n"; // 每个条目之间添加两个换行符
        }
    }

    // 返回一个只有一个字符串元素的vector，该元素包含所有格式化的记录，每两条记录之间有两个换行符。
    return {combinedRankings};
}






//the main part of the window
int main(){
    string playername;
    bool leaderboardshowedcuzgamewin=false;
    Timer timer;
    bool leaderboardActive = false;
    bool isPaused = false;
    sf::Texture play;
    play.loadFromFile("files/images/play.png");
// 在main函数之前或者游戏类中定义这些变量
    // 数字纹理和精灵
    int flagCount =0;
    sf::Texture digitsTexture;
    if (!digitsTexture.loadFromFile("files/images/digits.png")) {
        // handle error
    }
vector<string> players= nameranking();
    vector<string> player1= formatRankingsForDisplay(players);
// 我们需要4个精灵：三个用于显示数字，一个用于可能的负号
    sf::Sprite digitSprites[4];
    for (int i = 0; i < 4; ++i) {
        digitSprites[i].setTexture(digitsTexture);
        // 位置根据你的需求设置
        digitSprites[i].setPosition(32 * i, 0);
    }

    bool debugMode = false;
    bool gameOver = false;
    bool gameWon = false;
//all different texture
    sf::Texture textureHidden;
    sf::Texture textureRevealed;
    sf::Texture textureMine;
    sf::Texture textureFlag;
    textureHidden.loadFromFile("files/images/tile_hidden.png");
    textureRevealed.loadFromFile("files/images/tile_revealed.png");
    textureMine.loadFromFile("files/images/mine.png");
    textureFlag.loadFromFile("files/images/flag.png");
//background
    sf::Sprite background;
    vector<sf::Texture> textureNumbers;
    for(int i=1; i<9; i++){
        sf::Texture text;
        string temp="files/images/number_"+ to_string(i);
        temp=temp+".png";
        text.loadFromFile(temp);
        textureNumbers.push_back(text);
    }

    //reading data and getting rowcount, colcount, and minenumbers
    ifstream readingfile("files/config.cfg");

    string colCounts; string rowCounts; string mices;
    getline(readingfile, colCounts);
    getline(readingfile, rowCounts);
    getline(readingfile, mices);

    int rowCount=stoi(rowCounts);
    int colCount=stoi(colCounts);
    int mice=stoi(mices);
    int minecount=mice;
    bool gamestart=false;
    // set the cursor only for fun
    float height=(rowCount*32)+100; float width=colCount*32;
    sf::Texture textureloseface;
    sf::Sprite spiratehappyface;
    textureloseface.loadFromFile("files/images/face_lose.png");
    spiratehappyface.setTexture(textureloseface);
    spiratehappyface.scale(0.5, 0.5);
    spiratehappyface.setOrigin(70, 70);

    // set the cursor for the assignment
    //first creates the cursor
    sf::RectangleShape cursor(sf::Vector2f(2, 20));
    cursor.setOrigin(1,9);
    cursor.setFillColor(sf::Color(176, 224, 230));
    //this is the text part
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    //the first text part
    sf::Text biggettext;
    biggettext.setFont(font);
    biggettext.getLocalBounds();
    biggettext.setString("WELCOME TO MINESWEEPER!");
    biggettext.setCharacterSize(24);
    biggettext.setFillColor(sf::Color::White);
    biggettext.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(biggettext,width/2, height/2-150);
    //the second text part
    sf::Text nameline;
    nameline.setFont(font);
    nameline.setString("Enter your name:");
    nameline.setCharacterSize(20);
    nameline.setFillColor(sf::Color::White);
    nameline.setStyle(sf::Text::Bold);
    nameline.getLocalBounds();
    setText(nameline,width/2, height/2-75);

    //gave name input part
    sf::String playerInput;
    sf::Text playerText;
    playerText.setFont(font);
    playerText.setCharacterSize(18);
    playerText.setFillColor(sf::Color::Yellow);
    playerText.getLocalBounds();
    // the leaderboard part
    sf::String title;
    sf::Text titletext;
    titletext.setFont(font);
    titletext.getLocalBounds();
    titletext.setString("LEADERBOARD");
    titletext.setCharacterSize(20);
    titletext.setFillColor(sf::Color::White);
    titletext.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(titletext,(colCount*16)/2, (rowCount*16+50)/2-120);


//set all the different button
    //happy face button
    sf::Texture coolface; coolface.loadFromFile("files/images/face_win.png");
    sf::Texture happytexture;
    sf::Sprite  happySpite;
    happytexture.loadFromFile("files/images/face_happy.png");
    happySpite.setTexture(happytexture);
    float happyx=(colCount/2*32)-32;
    float happyy=(rowCount+0.5)*32;
    happySpite.setPosition(happyx,happyy);

    //debug buttom
    sf::Texture debugtexture;
    debugtexture.loadFromFile("files/images/debug.png");
    sf::Sprite debugSprite;
    debugSprite.setTexture(debugtexture);
    float debugxposition=colCount*32-304;
    float debugyposition=32*(rowCount+0.5);
    debugSprite.setPosition(debugxposition,debugyposition);

    //pause and play button
    sf::Texture pausetexture;
    pausetexture.loadFromFile("files/images/pause.png");
    sf::Sprite pausesprite;
    pausesprite.setTexture(pausetexture);
    float pausex=colCount*32-240;
    float pausey=32*(rowCount+0.5);
    pausesprite.setPosition(pausex,pausey);
    //leaderbord buttom
    sf::Texture leaderbordtexture;
    leaderbordtexture.loadFromFile("files/images/leaderboard.png");
    sf::Sprite leaderbordsprite;
    leaderbordsprite.setTexture(leaderbordtexture);
    float leaderbordx=colCount*32-176;
    float leaderbordy=32*(rowCount+0.5);
    leaderbordsprite.setPosition(leaderbordx,leaderbordy);


    // TODO: 更新显示计时器的代码，具体代码见下文

//the hellowindow part
    sf::RenderWindow HelloWindow(sf::VideoMode(width,height), "Best game all over the world!", sf::Style::Titlebar | sf::Style::Close);
    while(HelloWindow.isOpen()){
        int count=0;
        sf::Mouse mouse;
        sf::Event event;

        //事件发生的地方
        while(HelloWindow.pollEvent(event)) {
            if (event.type == sf::Event::TextEntered) {
                //this is the situation when only give A-a;
                if (((event.text.unicode >= 'A' and event.text.unicode <= 'Z')
                     or (event.text.unicode >= 'a' and event.text.unicode <= 'z')) and playerInput.getSize() < 10) {
                    if (playerInput.getSize() == 0) {
                        if (event.text.unicode >= 'a' and event.text.unicode <= 'z') {
                            playerInput += event.text.unicode - 32;
                            playerText.setString(playerInput);
                            playername+=(event.text.unicode - 32);
                            cout<<playername<<endl;
                        } else {
                            playerInput += event.text.unicode;
                            playerText.setString(playerInput);
                            playername+=event.text.unicode;
                            cout<<playername<<endl;

                        }
                    } else if (event.text.unicode >= 'A' and event.text.unicode <= 'Z') {
                        playerInput += event.text.unicode + 32;
                        playername+=(event.text.unicode+32);
                        cout<<playername<<endl;

                        playerText.setString(playerInput);
                    } else {
                        playerInput += event.text.unicode;
                        playername+=(event.text.unicode);
                        playerText.setString(playerInput);                            cout<<playername<<endl;

                    }
                    setText(playerText, width / 2, height / 2 - 45);
                    sf::FloatRect textRect = playerText.getLocalBounds();
                    cursor.setPosition(width / 2 + textRect.width / 2 + 5, height / 2 - 45);

                }
            }
            //detete
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (!playerInput.isEmpty()) {
                        playerInput.erase(playerInput.getSize() - 1);
                        playername.erase(playername.size() - 1); // Remove the last character from playername
                        playerText.setString(playerInput);
                        setText(playerText, width / 2, height / 2 - 45);
                    }
                }
            }
            sf::FloatRect textRect = playerText.getLocalBounds();
            cursor.setPosition(width / 2 + textRect.width / 2 + 5, height / 2 - 45);  // 更新位置

            //the enger key
            if(event.type==sf::Event::KeyPressed and !playerInput.isEmpty()) {
                if(event.key.code ==sf::Keyboard::Enter){
                    HelloWindow.close(); gamestart=true;
                }

            }
            if (event.type == sf::Event::Closed) {HelloWindow.close();}

        }

        //this is the fun curser part
        spiratehappyface.setPosition(mouse.getPosition(HelloWindow).x,mouse.getPosition(HelloWindow).y);
        //this is the last main part
        HelloWindow.clear(sf::Color(209, 186, 116));
        HelloWindow.draw(spiratehappyface); //set curser
        HelloWindow.draw(biggettext); //set the biggesttext
        HelloWindow.draw(nameline); //set the nameline
        HelloWindow.draw(playerText);
        HelloWindow.draw(cursor);
        HelloWindow.display();
    }


    if(gamestart==true){
        //construct the board
        vector<vector<Tile>> board;
        for(int i=0; i<rowCount; i++){
            vector<Tile> raw;
            for(int j=0; j<colCount; j++){
                raw.push_back(Tile());}board.push_back(raw);}
//set up the mine
        putmine(rowCount, colCount, mice, board);
        sf::RenderWindow GameWindow(sf::VideoMode(width, height), "Game becomes competitive now!", sf::Style::Titlebar | sf::Style::Close);
        sf::Texture hiddentiletexture;
        hiddentiletexture.loadFromFile("files/images/tile_hidden.png");
        for(int i=0; i<rowCount; i++){
            for(int j=0; j<colCount; j++) {
                board[i][j].spriteTile.setTexture(hiddentiletexture);
                board[i][j].spriteTile.setPosition(j * 32.f,i * 32.f);
            }
        }
        timer.start();
        bool wasPausedBeforeLeaderboard = false;
        while(GameWindow.isOpen()) {
            timer.getElapsedTime();
            sf::Mouse mouse;
            sf::Event event;
            int mousex = mouse.getPosition(GameWindow).x;
            int mousey = mouse.getPosition(GameWindow).y;
//draw the board part
//draw the hidden one
//the event part, and you can get the location if you click on some thing
            while(GameWindow.pollEvent(event)){
                // 在游戏开始或重新开始时
                // 在事件循环中检测鼠标点击事件
                if(event.type == sf::Event::MouseButtonPressed && !gameOver) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(GameWindow);
                    // 检查是否点击了debug按钮
                    if (!isPaused and debugSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        debugMode = !debugMode; // 切换debug模式
                        for (int i = 0; i < rowCount; ++i) {
                            for (int j = 0; j < colCount; ++j) {
                                if (debugMode) {
                                    if (board[i][j].isMine) {
                                        // 如果是地雷且当前在debug模式下，设置为地雷纹理
                                        board[i][j].spriteTile.setTexture(textureMine);
                                    }
                                } else {
                                    // 如果不是debug模式，恢复到它们的原始状态
                                    // 这里调用updateTexture函数来确保每个瓦片的显示是正确的
                                    board[i][j].updateTexture(textureHidden, textureRevealed, &textureNumbers[0],
                                                              textureMine, textureFlag);
                                }
                            }
                        }
                        }
                }


                if(event.type==sf::Event::Closed and !leaderboardActive){
                    GameWindow.close();
                }
                //if you click within the tile
                if(event.type==sf::Event::MouseButtonPressed and !isPaused and !gameOver and !leaderboardActive){
                    sf::Vector2i mousePos = sf::Mouse::getPosition(GameWindow);
                    for (int i = 0; i < rowCount; ++i) {
                        for (int j = 0; j < colCount; ++j) {
                            sf::FloatRect bounds = board[i][j].spriteTile.getGlobalBounds();
                            if (bounds.contains(mousePos.x, mousePos.y)) {
                                int colonclicked = j;
                                int rowclicked = i;
                                //if it is left click and aim to click the tile
                                if(event.mouseButton.button == sf::Mouse::Left){
                                    board[rowclicked][colonclicked].reveal(board, rowclicked, colonclicked, rowCount, colCount,textureHidden, textureRevealed, &textureNumbers[0], textureMine, textureFlag, gameOver, debugMode);
                                    checkWin(board);
                                    board[rowclicked][colonclicked].updateTexture(textureHidden, textureRevealed, &textureNumbers[0], textureMine, textureFlag);
                                    if(checkWin(board)){
                                        happySpite.setTexture(coolface);
                                        gameOver=true;
                                        gameWon=true;
                                    }
                                }
                                    //if it is right hand click
                                else if(event.mouseButton.button == sf::Mouse::Right){
                                    Tile& currentTile = board[rowclicked][colonclicked];
                                    if(!board[rowclicked][colonclicked].isRevealed){
                                        board[rowclicked][colonclicked].isFlagged = !board[rowclicked][colonclicked].isFlagged;
                                        if(currentTile.isFlagged) {
                                            flagCount++; // 如果放置了旗子，增加计数
                                        } else {
                                            flagCount--; // 如果移除了旗子，减少计数
                                        }}
                                    board[rowclicked][colonclicked].updateTexture(textureHidden, textureRevealed, &textureNumbers[0], textureMine, textureFlag);
                                    checkWin(board);
                                    board[rowclicked][colonclicked].updateTexture(textureHidden, textureRevealed, &textureNumbers[0], textureMine, textureFlag);
                                    if(checkWin(board)){
                                        happySpite.setTexture(coolface);
                                        gameOver=true;
                                        gameWon=true;
                                        string time= getTimerString(timer);
                                        if(playername[playername.size()-1]=='*'){playername=playername;                            cout<<playername<<endl;
                                        }

                                        else{playername=playername+"*";                            cout<<playername<<endl;
                                        }
                                        vector<string> temp1=nameranking(playername, time);
                                    }
                                }
                            }
                        }

                    }}
                if (event.type == sf::Event::MouseButtonPressed and gameOver==false) {
                    if (pausesprite.getGlobalBounds().contains(mouse.getPosition(GameWindow).x, mouse.getPosition(GameWindow).y)) {
                        // 切换暂停状态
                        isPaused = !isPaused;
                        if(isPaused){pausesprite.setTexture(play); timer.pause();}
                        else{pausesprite.setTexture(pausetexture);timer.resume();}
                        // 如果游戏暂停，停止计时器；如果游戏恢复，继续计时器
                    }}
                //如果游戏赢了，update参赛者的名字



                if (event.type == sf::Event::MouseButtonPressed and !isPaused) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(GameWindow);
                    if (happySpite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        timer.reset();
                        timer.start();
                        leaderboardshowedcuzgamewin=false;
                        resetGame(board, rowCount, colCount, minecount, textureHidden, gameOver, gameWon, flagCount);
                        happySpite.setTexture(happytexture);
                    }}
                //leaderboard
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(GameWindow);
                    //leaderboard
                    if (leaderbordsprite.getGlobalBounds().contains(mousePos.x, mousePos.y) or (leaderboardshowedcuzgamewin==false and gameWon)) {
                        vector<string> temp1=nameranking();
                        vector<string>temp2=formatRankingsForDisplay(temp1);
                        vector<sf::Text> temp11=nametranslator(temp2, font, colCount, rowCount);

                        timer.pause();
                        bool temp=isPaused;
                        isPaused;

                        leaderboardshowedcuzgamewin=true;
                        leaderboardActive = true;
                        wasPausedBeforeLeaderboard = isPaused;
                        isPaused = true;
                        sf::RenderWindow leaderboardWindow(sf::VideoMode(colCount*16, rowCount*16+50), "Leaderboard", sf::Style::Titlebar | sf::Style::Close);
                        sf::RenderWindow* leaderbow=&leaderboardWindow;
                        while (leaderboardWindow.isOpen()) {
                            sf::Event event;
                            while (leaderboardWindow.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    leaderboardWindow.close();
                                    if(!gameWon){
                                        if(!temp){timer.resume();isPaused= temp;}
                                        else if(temp){isPaused=temp;}
                                    }
                                    isPaused = wasPausedBeforeLeaderboard;
                                    leaderboardActive = false;
                                    if(GameWindow.pollEvent(event))
                                    {
                                        if (event.type == sf::Event::Closed) {
                                            continue;
                                        }
                                    }
                                }

                                if(leaderboardActive and !gameOver){
                                    for(int i=0; i<rowCount; i++){
                                        for(int j=0; j<colCount; j++) {
                                            sf::Sprite background;
                                            background.setTexture(textureRevealed);
                                            background.setPosition(j * 32.f,i * 32.f);
                                            GameWindow.draw(background);

                                        } }

                                }
                                leaderboardWindow.clear(sf::Color(230, 206, 172));
                                leaderboardWindow.draw(titletext);
                                for(int i=0; i<temp11.size(); i++)

                                {leaderboardWindow.draw(temp11[i]);}
                                leaderboardWindow.display();
                                GameWindow.display();
                            }
                        }

                    }


                    //leaderboard part
                }





            }




            GameWindow.clear(sf::Color(214, 213, 183));
            GameWindow.draw(debugSprite);
            GameWindow.draw(happySpite);
            GameWindow.draw(pausesprite);
            GameWindow.draw(leaderbordsprite);
            //draw the game board
            for(int i=0; i<rowCount; i++){
                for(int j=0; j<colCount; j++) {
                    sf::Sprite background;
                    background.setTexture(textureRevealed);
                    background.setPosition(j * 32.f,i * 32.f);
                    GameWindow.draw(background);
                    GameWindow.draw(board[i][j].spriteTile);

                }
            }
            if (gameOver) {
                timer.pause();
                if (!gameWon) {
                    // 更换为输掉游戏的表情
                    happySpite.setTexture(textureloseface);
                }
            }
            if (debugMode) {

                for (int i = 0; i < rowCount; i++) {
                    for (int j = 0; j < colCount; j++) {
                        if (board[i][j].isMine) {
                            board[i][j].spriteTile.setTexture(textureMine);

                        }
                    }
                }
            }
            updateCounterDisplay(minecount, flagCount, GameWindow, digitSprites, rowCount);
            updateTimerDisplay(timer, digitSprites, GameWindow, colCount,rowCount);
            GameWindow.display();

        }

    }
    return 0;
}
