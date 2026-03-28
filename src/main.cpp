#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include "HexGrid.hpp"
#include "Algorithms/AStar.hpp"
#include "Algorithms/Dijkstras.hpp"
using namespace sf;
using namespace std;

const float PI = 3.14159265f;

enum CellState { UNSEEN, WALL, START, END, TRAVERSED, CANDIDATE, PATH, TRAVERSED2, PATH2 };

enum Screen { MENU, MAZE };

Color getColor(CellState state) {
    switch(state) {
        case START:      return Color(34, 197, 94);
        case END:        return Color(239, 68, 68);
        case TRAVERSED:  return Color(30, 64, 175);
        case CANDIDATE:  return Color(147, 197, 253);
        case PATH:       return Color(250, 204, 21);
        case WALL:       return Color(50, 50, 50);
        case TRAVERSED2: return Color(128, 0, 128);
        case PATH2:      return Color(200, 0, 255);
        default:         return Color::White;
    }
}

ConvexShape makeHex(float centerX, float centerY, float size, Color color) {
    ConvexShape hex(6);
    for (int i = 0; i < 6; i++) {
        float angle = (PI / 180.f) * (60.f * i + 30.f);
        hex.setPoint(i, {centerX + size * cos(angle), centerY + size * sin(angle)});
    }
    hex.setFillColor(color);
    hex.setOutlineColor(Color::Black);
    hex.setOutlineThickness(1.f);
    return hex;
}

RectangleShape makeButton(float x, float y, float w, float h) {
    RectangleShape btn(Vector2f(w, h));
    btn.setPosition({x, y});
    btn.setFillColor(Color(70, 70, 70));
    btn.setOutlineColor(Color::White);
    btn.setOutlineThickness(1.f);
    return btn;
}

bool isClicked(RectangleShape& btn, Vector2f mousePos) {
    return btn.getGlobalBounds().contains(mousePos);
}

void centerText(Text& text, RectangleShape& btn) {
    FloatRect textBounds = text.getLocalBounds();
    FloatRect btnBounds  = btn.getGlobalBounds();
    text.setPosition({
        btnBounds.position.x + (btnBounds.size.x - textBounds.size.x) / 2.f - textBounds.position.x,
        btnBounds.position.y + (btnBounds.size.y - textBounds.size.y) / 2.f - textBounds.position.y
    });
}

int main() {
    RenderWindow window(VideoMode({1000, 750}), "Hex Maze");

    Font font;
    font.openFromFile("../resources/Helvetica.ttf");

    Screen currentScreen = MENU;

    int gridSizeX = 10;
    int gridSizeY = 10;
    int selectedAlgo = 0;

    RectangleShape sizeXDownBtn = makeButton(280.f, 200.f, 40.f, 40.f);
    RectangleShape sizeXUpBtn   = makeButton(480.f, 200.f, 40.f, 40.f);
    RectangleShape sizeYDownBtn = makeButton(280.f, 260.f, 40.f, 40.f);
    RectangleShape sizeYUpBtn   = makeButton(480.f, 260.f, 40.f, 40.f);
    RectangleShape algoABtn     = makeButton(200.f, 330.f, 120.f, 40.f);
    RectangleShape algoDBtn     = makeButton(340.f, 330.f, 120.f, 40.f);
    RectangleShape algoBothBtn  = makeButton(480.f, 330.f, 120.f, 40.f);
    RectangleShape startBtn     = makeButton(350.f, 420.f, 200.f, 50.f);

    float size = 30.f;
    float w = size * sqrt(3.f);
    int rows = 10, cols = 10;

    vector<vector<CellState>> frames;
    int currentFrame = 0;
    bool isPlaying = false;
    float animSpeed = 0.5f;
    float timeSinceLastFrame = 0.f;
    Clock clock;

    int traversedCount = 0, candidateCount = 0, pathLength = 0, executionTime = 0;
    int traversedCount2 = 0, pathLength2 = 0;

    View gridView = window.getDefaultView();
    gridView.setViewport(FloatRect({0.f, 0.f}, {1.f, 0.80f}));
    bool isDragging = false;
    Vector2f lastMousePos;
    View uiView = window.getDefaultView();

    float sliderMin = 150.f, sliderMax = 500.f, sliderY = 710.f, sliderX = 325.f;
    float zoomLevel = 1.f;
    bool isDraggingSlider = false;

    float speedSliderMin = 150.f, speedSliderMax = 500.f, speedSliderY = 680.f, speedSliderX = 325.f;
    bool isDraggingSpeedSlider = false;

    float btnW = 130.f, btnH = 30.f, btnY = 610.f;
    RectangleShape pauseBtn = makeButton(50.f,  btnY, btnW, btnH);
    RectangleShape playBtn  = makeButton(200.f, btnY, btnW, btnH);
    RectangleShape stepBtn  = makeButton(350.f, btnY, btnW, btnH);

    RectangleShape statsPanel(Vector2f(190.f, 110.f));
    statsPanel.setFillColor(Color(50, 50, 50));
    statsPanel.setOutlineColor(Color(30, 64, 175));
    statsPanel.setOutlineThickness(2.f);

    RectangleShape statsPanel2(Vector2f(190.f, 110.f));
    statsPanel2.setFillColor(Color(50, 50, 50));
    statsPanel2.setOutlineColor(Color(128, 0, 128));
    statsPanel2.setOutlineThickness(2.f);

    int algo1ExecutionTime = 0, algo2ExecutionTime = 0;
    int algo1PathLength = 0,    algo2PathLength = 0;
    int algo1Traversed = 0,     algo2Traversed = 0;

    while (window.isOpen()) {

        float dt = clock.restart().asSeconds();

        if (currentScreen == MAZE) {
            if (isPlaying && currentFrame < (int)frames.size() - 1) {
                timeSinceLastFrame += dt;
                if (timeSinceLastFrame >= animSpeed) {
                    timeSinceLastFrame = 0.f;
                    currentFrame++;
                }
            }
            traversedCount = 0; candidateCount = 0; pathLength = 0;
            traversedCount2 = 0; pathLength2 = 0;
            for (auto& cell : frames[currentFrame]) {
                if (cell == TRAVERSED)  traversedCount++;
                if (cell == TRAVERSED2) traversedCount2++;
                if (cell == CANDIDATE)  candidateCount++;
                if (cell == PATH)       pathLength++;
                if (cell == PATH2)      pathLength2++;
            }
        }

        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* mouseDown = event->getIf<Event::MouseButtonPressed>()) {
                Vector2f mousePos = {(float)mouseDown->position.x, (float)mouseDown->position.y};

                if (currentScreen == MENU) {
                    if (isClicked(sizeXDownBtn, mousePos) && gridSizeX > 5)    gridSizeX--;
                    if (isClicked(sizeXUpBtn,   mousePos) && gridSizeX < 1000) gridSizeX++;
                    if (isClicked(sizeYDownBtn, mousePos) && gridSizeY > 5)    gridSizeY--;
                    if (isClicked(sizeYUpBtn,   mousePos) && gridSizeY < 1000) gridSizeY++;
                    if (isClicked(algoABtn,     mousePos)) selectedAlgo = 1;
                    if (isClicked(algoDBtn,     mousePos)) selectedAlgo = 2;
                    if (isClicked(algoBothBtn,  mousePos)) selectedAlgo = 0;

                    if (isClicked(startBtn, mousePos)) {
                        rows = gridSizeY;
                        cols = gridSizeX;
                        w = size * sqrt(3.f);
                        frames.clear();
                        currentFrame = 0;
                        isPlaying = false;
                        algo1ExecutionTime = 0; algo2ExecutionTime = 0;
                        algo1PathLength = 0;    algo2PathLength = 0;
                        algo1Traversed = 0;     algo2Traversed = 0;

                        HexGrid grid({cols, rows});
                        Vec2d startPos = {0, 0};
                        Vec2d endPos   = {cols - 1, rows - 1};

                        auto buildFrames = [&](Algorithm& algo, int algoIndex) {
                            while (!algo.getFoundPath().has_value()) {
                                algo.nextIteration();

                                vector<CellState> frame(rows * cols, UNSEEN);

                                for (int r = 0; r < rows; r++)
                                    for (int c = 0; c < cols; c++)
                                        if (!grid.getCell({c, r}))
                                            frame[r * cols + c] = WALL;

                                for (auto& node : algo.getSearchedNodes())
                                    frame[node.second * cols + node.first] = algoIndex == 0 ? TRAVERSED : TRAVERSED2;

                                if (algo.getFoundPath().has_value())
                                    for (auto& node : algo.getFoundPath().value())
                                        frame[node.second * cols + node.first] = algoIndex == 0 ? PATH : PATH2;

                                frame[0] = START;
                                frame[rows * cols - 1] = END;
                                frames.push_back(frame);
                            }
                            if (algoIndex == 0) {
                                algo1ExecutionTime = algo.getExecutionTime();
                                algo1Traversed = algo.getSearchedNodes().size();
                                algo1PathLength = algo.getFoundPath().value().size();
                            } else {
                                algo2ExecutionTime = algo.getExecutionTime();
                                algo2Traversed = algo.getSearchedNodes().size();
                                algo2PathLength = algo.getFoundPath().value().size();
                            }
                            executionTime = algo.getExecutionTime();
                        };

                        if (selectedAlgo == 1) {
                            AStar algo(startPos, endPos, &grid);
                            buildFrames(algo, 0);
                        } else if (selectedAlgo == 2) {
                            Dijkstras algo(startPos, endPos, &grid);
                            buildFrames(algo, 0);
                        } else {
                            AStar algoA(startPos, endPos, &grid);
                            buildFrames(algoA, 0);
                            Dijkstras algoD(startPos, endPos, &grid);
                            buildFrames(algoD, 1);
                        }

                        gridView = window.getDefaultView();
                        gridView.setViewport(FloatRect({0.f, 0.f}, {1.f, 0.80f}));
                        zoomLevel = 1.f;
                        currentScreen = MAZE;
                    }
                }

                if (currentScreen == MAZE) {
                    if (isClicked(pauseBtn, mousePos)) isPlaying = false;
                    if (isClicked(playBtn,  mousePos)) isPlaying = true;
                    if (isClicked(stepBtn,  mousePos)) {
                        if (currentFrame < (int)frames.size() - 1)
                            currentFrame++;
                    }
                    if (mousePos.y > 668.f && mousePos.y < 693.f &&
                        mousePos.x > speedSliderMin && mousePos.x < speedSliderMax)
                        isDraggingSpeedSlider = true;
                    else if (mousePos.y > 698.f && mousePos.x > sliderMin && mousePos.x < sliderMax)
                        isDraggingSlider = true;
                    else if (mousePos.y < 600.f) {
                        isDragging = true;
                        lastMousePos = window.mapPixelToCoords(mouseDown->position, gridView);
                    }
                }
            }

            if (const auto* mouseUp = event->getIf<Event::MouseButtonReleased>()) {
                isDragging = false;
                isDraggingSlider = false;
                isDraggingSpeedSlider = false;
            }

            if (const auto* mouseMove = event->getIf<Event::MouseMoved>()) {
                if (isDraggingSpeedSlider) {
                    speedSliderX = max(speedSliderMin, min(speedSliderMax, (float)mouseMove->position.x));
                    float t = (speedSliderX - speedSliderMin) / (speedSliderMax - speedSliderMin);
                    animSpeed = 2.0f - t * 1.9f;
                }
                if (isDraggingSlider) {
                    sliderX = max(sliderMin, min(sliderMax, (float)mouseMove->position.x));
                    float t = (sliderX - sliderMin) / (sliderMax - sliderMin);
                    float newZoom = 0.1f + t * 3.9f;
                    gridView.zoom(newZoom / zoomLevel);
                    zoomLevel = newZoom;
                }
                if (isDragging) {
                    Vector2f newPos = window.mapPixelToCoords(mouseMove->position, gridView);
                    gridView.move(lastMousePos - newPos);
                    lastMousePos = window.mapPixelToCoords(mouseMove->position, gridView);
                }
            }
        }

        window.clear(Color(30, 30, 30));

        if (currentScreen == MENU) {
            window.setView(uiView);

            RectangleShape mazeBorder(Vector2f(998.f, 598.f));
            mazeBorder.setPosition({1.f, 1.f});
            mazeBorder.setFillColor(Color::Transparent);
            mazeBorder.setOutlineColor(Color(100, 100, 100));
            mazeBorder.setOutlineThickness(2.f);
            window.draw(mazeBorder);

            RectangleShape divider(Vector2f(1000.f, 2.f));
            divider.setPosition({0.f, 600.f});
            divider.setFillColor(Color(100, 100, 100));
            window.draw(divider);

            Text title(font, "Happy Hexagon Maze", 36);
            FloatRect tb = title.getLocalBounds();
            title.setPosition({500.f - tb.size.x / 2.f, 100.f});
            title.setFillColor(Color::White);
            window.draw(title);

            Text xLabel(font, "Grid X:", 18);
            xLabel.setPosition({200.f, 208.f});
            xLabel.setFillColor(Color::White);
            window.draw(xLabel);

            window.draw(sizeXDownBtn);
            Text xDownText(font, "-", 20);
            xDownText.setFillColor(Color::White);
            centerText(xDownText, sizeXDownBtn);
            window.draw(xDownText);

            Text xNum(font, to_string(gridSizeX), 22);
            FloatRect xnb = xNum.getLocalBounds();
            xNum.setPosition({400.f - xnb.size.x / 2.f, 208.f});
            xNum.setFillColor(Color(250, 204, 21));
            window.draw(xNum);

            window.draw(sizeXUpBtn);
            Text xUpText(font, "+", 20);
            xUpText.setFillColor(Color::White);
            centerText(xUpText, sizeXUpBtn);
            window.draw(xUpText);

            Text yLabel(font, "Grid Y:", 18);
            yLabel.setPosition({200.f, 268.f});
            yLabel.setFillColor(Color::White);
            window.draw(yLabel);

            window.draw(sizeYDownBtn);
            Text yDownText(font, "-", 20);
            yDownText.setFillColor(Color::White);
            centerText(yDownText, sizeYDownBtn);
            window.draw(yDownText);

            Text yNum(font, to_string(gridSizeY), 22);
            FloatRect ynb = yNum.getLocalBounds();
            yNum.setPosition({400.f - ynb.size.x / 2.f, 268.f});
            yNum.setFillColor(Color(250, 204, 21));
            window.draw(yNum);

            window.draw(sizeYUpBtn);
            Text yUpText(font, "+", 20);
            yUpText.setFillColor(Color::White);
            centerText(yUpText, sizeYUpBtn);
            window.draw(yUpText);

            Text algoLabel(font, "Algorithm:", 18);
            algoLabel.setPosition({200.f, 338.f});
            algoLabel.setFillColor(Color::White);
            window.draw(algoLabel);

            algoABtn.setFillColor(selectedAlgo == 1 ? Color(30, 64, 175) : Color(70, 70, 70));
            algoDBtn.setFillColor(selectedAlgo == 2 ? Color(30, 64, 175) : Color(70, 70, 70));
            algoBothBtn.setFillColor(selectedAlgo == 0 ? Color(30, 64, 175) : Color(70, 70, 70));

            window.draw(algoABtn);
            window.draw(algoDBtn);
            window.draw(algoBothBtn);

            Text aText(font, "A*", 16);
            aText.setFillColor(Color::White);
            centerText(aText, algoABtn);
            window.draw(aText);

            Text dText(font, "Dijkstra", 16);
            dText.setFillColor(Color::White);
            centerText(dText, algoDBtn);
            window.draw(dText);

            Text bothText(font, "Both", 16);
            bothText.setFillColor(Color::White);
            centerText(bothText, algoBothBtn);
            window.draw(bothText);

            window.draw(startBtn);
            Text startText(font, "Start", 22);
            startText.setFillColor(Color::White);
            centerText(startText, startBtn);
            window.draw(startText);
        }

        if (currentScreen == MAZE) {
            window.setView(gridView);
            for (int row = 0; row < rows; row++) {
                for (int col = 0; col < cols; col++) {
                    float centerX = 60.f + col * w + (row % 2 == 1 ? w / 2.f : 0.f);
                    float centerY = 60.f + row * (size * 1.5f);
                    CellState state = frames[currentFrame][row * cols + col];
                    auto hex = makeHex(centerX, centerY, size, getColor(state));
                    window.draw(hex);
                }
            }

            window.setView(uiView);

            RectangleShape mazeBorder(Vector2f(998.f, 598.f));
            mazeBorder.setPosition({1.f, 1.f});
            mazeBorder.setFillColor(Color::Transparent);
            mazeBorder.setOutlineColor(Color(100, 100, 100));
            mazeBorder.setOutlineThickness(2.f);
            window.draw(mazeBorder);

            RectangleShape divider(Vector2f(1000.f, 2.f));
            divider.setPosition({0.f, 600.f});
            divider.setFillColor(Color(100, 100, 100));
            window.draw(divider);

            window.draw(pauseBtn);
            window.draw(playBtn);
            window.draw(stepBtn);

            Text pauseText(font, "Pause", 13);
            pauseText.setFillColor(Color::White);
            centerText(pauseText, pauseBtn);
            window.draw(pauseText);

            Text playText(font, "Play", 13);
            playText.setFillColor(Color::White);
            centerText(playText, playBtn);
            window.draw(playText);

            Text stepText(font, "Step Forward", 13);
            stepText.setFillColor(Color::White);
            centerText(stepText, stepBtn);
            window.draw(stepText);

            Text status(font, isPlaying ? "Status: Playing" : "Status: Paused", 14);
            status.setPosition({10.f, 720.f});
            status.setFillColor(isPlaying ? Color(34, 197, 94) : Color(239, 68, 68));
            window.draw(status);

            Text frameText(font, "Frame: " + to_string(currentFrame + 1) + "/" + to_string(frames.size()), 14);
            frameText.setPosition({10.f, 735.f});
            frameText.setFillColor(Color::White);
            window.draw(frameText);

            RectangleShape speedTrack(Vector2f(speedSliderMax - speedSliderMin, 4.f));
            speedTrack.setPosition({speedSliderMin, speedSliderY});
            speedTrack.setFillColor(Color(150, 150, 150));
            window.draw(speedTrack);

            CircleShape speedHandle(10.f);
            speedHandle.setOrigin({10.f, 10.f});
            speedHandle.setPosition({speedSliderX, speedSliderY + 2.f});
            speedHandle.setFillColor(Color::White);
            window.draw(speedHandle);

            Text speedLabel(font, "Speed", 14);
            speedLabel.setPosition({speedSliderMin - 65.f, speedSliderY - 8.f});
            speedLabel.setFillColor(Color::White);
            window.draw(speedLabel);

            RectangleShape track(Vector2f(sliderMax - sliderMin, 4.f));
            track.setPosition({sliderMin, sliderY});
            track.setFillColor(Color(150, 150, 150));
            window.draw(track);

            CircleShape handle(10.f);
            handle.setOrigin({10.f, 10.f});
            handle.setPosition({sliderX, sliderY + 2.f});
            handle.setFillColor(Color::White);
            window.draw(handle);

            Text zoomLabel(font, "Zoom", 14);
            zoomLabel.setPosition({sliderMin - 60.f, sliderY - 8.f});
            zoomLabel.setFillColor(Color::White);
            window.draw(zoomLabel);

            if (selectedAlgo == 0) {
                statsPanel.setPosition({560.f, 605.f});
                window.draw(statsPanel);

                Text statsTitle(font, "A* Stats", 13);
                statsTitle.setPosition({570.f, 612.f});
                statsTitle.setFillColor(Color(147, 197, 253));
                window.draw(statsTitle);

                Text s1(font, "Traversed: " + to_string(traversedCount), 12);
                s1.setPosition({570.f, 630.f});
                s1.setFillColor(Color(200, 200, 200));
                window.draw(s1);

                Text s2(font, "Path length: " + to_string(pathLength), 12);
                s2.setPosition({570.f, 647.f});
                s2.setFillColor(Color(200, 200, 200));
                window.draw(s2);

                Text s3(font, "Time: " + to_string(algo1ExecutionTime) + "ms", 12);
                s3.setPosition({570.f, 664.f});
                s3.setFillColor(Color(200, 200, 200));
                window.draw(s3);

                statsPanel2.setPosition({760.f, 605.f});
                window.draw(statsPanel2);

                Text statsTitle2(font, "Dijkstra Stats", 13);
                statsTitle2.setPosition({770.f, 612.f});
                statsTitle2.setFillColor(Color(200, 0, 255));
                window.draw(statsTitle2);

                Text d1(font, "Traversed: " + to_string(traversedCount2), 12);
                d1.setPosition({770.f, 630.f});
                d1.setFillColor(Color(200, 200, 200));
                window.draw(d1);

                Text d2(font, "Path length: " + to_string(pathLength2), 12);
                d2.setPosition({770.f, 647.f});
                d2.setFillColor(Color(200, 200, 200));
                window.draw(d2);

                Text d3(font, "Time: " + to_string(algo2ExecutionTime) + "ms", 12);
                d3.setPosition({770.f, 664.f});
                d3.setFillColor(Color(200, 200, 200));
                window.draw(d3);

            } else {
                statsPanel.setPosition({760.f, 605.f});
                window.draw(statsPanel);

                Text statsTitle(font, "Algorithm Stats", 13);
                statsTitle.setPosition({770.f, 612.f});
                statsTitle.setFillColor(Color::White);
                window.draw(statsTitle);

                Text statLine1(font, "Traversed: " + to_string(traversedCount), 12);
                statLine1.setPosition({770.f, 630.f});
                statLine1.setFillColor(Color(200, 200, 200));
                window.draw(statLine1);

                Text statLine2(font, "Candidates: " + to_string(candidateCount), 12);
                statLine2.setPosition({770.f, 647.f});
                statLine2.setFillColor(Color(200, 200, 200));
                window.draw(statLine2);

                Text statLine3(font, "Path length: " + to_string(pathLength), 12);
                statLine3.setPosition({770.f, 664.f});
                statLine3.setFillColor(Color(200, 200, 200));
                window.draw(statLine3);

                Text statLine4(font, "Time: " + to_string(executionTime) + "ms", 12);
                statLine4.setPosition({770.f, 681.f});
                statLine4.setFillColor(Color(200, 200, 200));
                window.draw(statLine4);
            }
        }

        window.display();
    }
}