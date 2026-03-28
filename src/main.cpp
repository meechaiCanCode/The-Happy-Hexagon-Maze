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

enum CellState { UNSEEN, WALL, START, END, TRAVERSED, CANDIDATE, PATH };

enum Screen { MENU, MAZE };

Color getColor(CellState state) {
    switch(state) {
        case START:     return Color(34, 197, 94);
        case END:       return Color(239, 68, 68);
        case TRAVERSED: return Color(30, 64, 175);
        case CANDIDATE: return Color(147, 197, 253);
        case PATH:      return Color(250, 204, 21);
        case WALL:      return Color(50, 50, 50);
        default:        return Color::White;
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
    RenderWindow window(VideoMode({800, 600}), "Hex Maze");

    Font font;
    font.openFromFile("/System/Library/Fonts/Helvetica.ttc");

    Screen currentScreen = MENU;

    int gridSize = 10;
    int selectedAlgo = 0;

    RectangleShape sizeDownBtn  = makeButton(280.f, 220.f, 40.f,  40.f);
    RectangleShape sizeUpBtn    = makeButton(480.f, 220.f, 40.f,  40.f);
    RectangleShape algoABtn     = makeButton(200.f, 310.f, 120.f, 40.f);
    RectangleShape algoDBtn     = makeButton(340.f, 310.f, 120.f, 40.f);
    RectangleShape algoBothBtn  = makeButton(480.f, 310.f, 120.f, 40.f);
    RectangleShape startBtn     = makeButton(300.f, 420.f, 200.f, 50.f);

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

    View gridView = window.getDefaultView();
    gridView.setViewport(FloatRect({0.f, 0.f}, {1.f, 0.82f}));
    bool isDragging = false;
    Vector2f lastMousePos;
    View uiView = window.getDefaultView();

    float sliderMin = 200.f, sliderMax = 600.f, sliderY = 560.f, sliderX = 400.f;
    float zoomLevel = 1.f;
    bool isDraggingSlider = false;

    float speedSliderMin = 200.f, speedSliderMax = 600.f, speedSliderY = 535.f, speedSliderX = 400.f;
    bool isDraggingSpeedSlider = false;

    float btnW = 130.f, btnH = 30.f, btnY = 500.f;
    RectangleShape pauseBtn = makeButton(50.f,  btnY, btnW, btnH);
    RectangleShape playBtn  = makeButton(200.f, btnY, btnW, btnH);
    RectangleShape stepBtn  = makeButton(350.f, btnY, btnW, btnH);

    RectangleShape statsPanel(Vector2f(180.f, 110.f));
    statsPanel.setPosition({600.f, 480.f});
    statsPanel.setFillColor(Color(50, 50, 50));
    statsPanel.setOutlineColor(Color::White);
    statsPanel.setOutlineThickness(1.f);

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
            for (auto& cell : frames[currentFrame]) {
                if (cell == TRAVERSED) traversedCount++;
                if (cell == CANDIDATE) candidateCount++;
                if (cell == PATH)      pathLength++;
            }
        }

        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* mouseDown = event->getIf<Event::MouseButtonPressed>()) {
                Vector2f mousePos = {(float)mouseDown->position.x, (float)mouseDown->position.y};

                if (currentScreen == MENU) {
                    if (isClicked(sizeDownBtn, mousePos) && gridSize > 5)  gridSize--;
                    if (isClicked(sizeUpBtn,   mousePos) && gridSize < 30) gridSize++;
                    if (isClicked(algoABtn,    mousePos)) selectedAlgo = 1;
                    if (isClicked(algoDBtn,    mousePos)) selectedAlgo = 2;
                    if (isClicked(algoBothBtn, mousePos)) selectedAlgo = 0;

                    if (isClicked(startBtn, mousePos)) {
                        rows = gridSize;
                        cols = gridSize;
                        w = size * sqrt(3.f);
                        frames.clear();
                        currentFrame = 0;
                        isPlaying = false;

                        HexGrid grid({cols, rows});
                        Vec2d startPos = {0, 0};
                        Vec2d endPos   = {cols - 1, rows - 1};

                        auto buildFrames = [&](Algorithm& algo) {
                            while (!algo.getFoundPath().has_value()) {
                                algo.nextIteration();

                                vector<CellState> frame(rows * cols, UNSEEN);
                                frame[0] = START;
                                frame[rows * cols - 1] = END;

                                for (int r = 0; r < rows; r++)
                                    for (int c = 0; c < cols; c++)
                                        if (!grid.getCell({c, r}))
                                            frame[r * cols + c] = WALL;

                                for (auto& node : algo.getSearchedNodes())
                                    frame[node.second * cols + node.first] = TRAVERSED;

                                if (algo.getFoundPath().has_value())
                                    for (auto& node : algo.getFoundPath().value())
                                        frame[node.second * cols + node.first] = PATH;

                                frames.push_back(frame);
                            }
                            executionTime = algo.getExecutionTime();
                        };

                        if (selectedAlgo == 1) {
                            AStar algo(startPos, endPos, &grid);
                            buildFrames(algo);
                        } else if (selectedAlgo == 2) {
                            Dijkstras algo(startPos, endPos, &grid);
                            buildFrames(algo);
                        } else {
                            AStar algoA(startPos, endPos, &grid);
                            buildFrames(algoA);
                            Dijkstras algoD(startPos, endPos, &grid);
                            buildFrames(algoD);
                        }

                        gridView = window.getDefaultView();
                        gridView.setViewport(FloatRect({0.f, 0.f}, {1.f, 0.82f}));
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
                    if (mousePos.y > 525.f && mousePos.y < 550.f &&
                        mousePos.x > speedSliderMin && mousePos.x < speedSliderMax)
                        isDraggingSpeedSlider = true;
                    else if (mousePos.y > 550.f && mousePos.x > sliderMin && mousePos.x < sliderMax)
                        isDraggingSlider = true;
                    else if (mousePos.y < 490.f) {
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
                    animSpeed = 0.1f + t * 1.9f;
                }
                if (isDraggingSlider) {
                    sliderX = max(sliderMin, min(sliderMax, (float)mouseMove->position.x));
                    float t = (sliderX - sliderMin) / (sliderMax - sliderMin);
                    float newZoom = 0.5f + t * 1.5f;
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

            RectangleShape mazeBorder(Vector2f(798.f, 488.f));
            mazeBorder.setPosition({1.f, 1.f});
            mazeBorder.setFillColor(Color::Transparent);
            mazeBorder.setOutlineColor(Color(100, 100, 100));
            mazeBorder.setOutlineThickness(2.f);
            window.draw(mazeBorder);

            RectangleShape divider(Vector2f(800.f, 2.f));
            divider.setPosition({0.f, 490.f});
            divider.setFillColor(Color(100, 100, 100));
            window.draw(divider);

            Text title(font, "Happy Hexagon Maze", 36);
            FloatRect tb = title.getLocalBounds();
            title.setPosition({400.f - tb.size.x / 2.f, 100.f});
            title.setFillColor(Color::White);
            window.draw(title);

            Text sizeLabel(font, "Grid Size:", 18);
            sizeLabel.setPosition({200.f, 228.f});
            sizeLabel.setFillColor(Color::White);
            window.draw(sizeLabel);

            window.draw(sizeDownBtn);
            Text downText(font, "-", 20);
            downText.setFillColor(Color::White);
            centerText(downText, sizeDownBtn);
            window.draw(downText);

            Text sizeNum(font, to_string(gridSize), 22);
            FloatRect snb = sizeNum.getLocalBounds();
            sizeNum.setPosition({400.f - snb.size.x / 2.f, 228.f});
            sizeNum.setFillColor(Color(250, 204, 21));
            window.draw(sizeNum);

            window.draw(sizeUpBtn);
            Text upText(font, "+", 20);
            upText.setFillColor(Color::White);
            centerText(upText, sizeUpBtn);
            window.draw(upText);

            Text algoLabel(font, "Algorithm:", 18);
            algoLabel.setPosition({200.f, 318.f});
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

            RectangleShape mazeBorder(Vector2f(798.f, 488.f));
            mazeBorder.setPosition({1.f, 1.f});
            mazeBorder.setFillColor(Color::Transparent);
            mazeBorder.setOutlineColor(Color(100, 100, 100));
            mazeBorder.setOutlineThickness(2.f);
            window.draw(mazeBorder);

            RectangleShape divider(Vector2f(800.f, 2.f));
            divider.setPosition({0.f, 490.f});
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
            status.setPosition({10.f, 495.f});
            status.setFillColor(isPlaying ? Color(34, 197, 94) : Color(239, 68, 68));
            window.draw(status);

            Text frameText(font, "Frame: " + to_string(currentFrame + 1) + "/" + to_string(frames.size()), 14);
            frameText.setPosition({10.f, 515.f});
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

            window.draw(statsPanel);

            Text statsTitle(font, "Algorithm Stats", 13);
            statsTitle.setPosition({610.f, 487.f});
            statsTitle.setFillColor(Color::White);
            window.draw(statsTitle);

            Text statLine1(font, "Traversed: " + to_string(traversedCount), 12);
            statLine1.setPosition({610.f, 507.f});
            statLine1.setFillColor(Color(200, 200, 200));
            window.draw(statLine1);

            Text statLine2(font, "Candidates: " + to_string(candidateCount), 12);
            statLine2.setPosition({610.f, 524.f});
            statLine2.setFillColor(Color(200, 200, 200));
            window.draw(statLine2);

            Text statLine3(font, "Path length: " + to_string(pathLength), 12);
            statLine3.setPosition({610.f, 541.f});
            statLine3.setFillColor(Color(200, 200, 200));
            window.draw(statLine3);

            Text statLine4(font, "Time: " + to_string(executionTime) + "ms", 12);
            statLine4.setPosition({610.f, 558.f});
            statLine4.setFillColor(Color(200, 200, 200));
            window.draw(statLine4);
        }

        window.display();
    }
}