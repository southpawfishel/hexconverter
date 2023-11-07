#include <string>
#include <unordered_map>
#include <unordered_set>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

std::unordered_set<char> hexChars{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
std::unordered_map<char, std::string> hexToBinary{
    {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"},
    {'6', "0110"}, {'7', "0111"}, {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
    {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
};
std::unordered_map<char, int> hexToDecimal{
    {'0', 0}, {'1', 1}, {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},  {'6', 6},  {'7', 7},
    {'8', 8}, {'9', 9}, {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
};

std::string hexStrToBinaryStr(const std::string hexStr) {
  std::string output;
  std::for_each(hexStr.begin(), hexStr.end(), [&output](auto const& c) { output.append(hexToBinary[c]); });
  return output;
}

std::string hexStrToDecimalStr(const std::string hexStr) {
  int decimal = 0;
  int pow16 = 1;
  for (auto iter = hexStr.rbegin(); iter != hexStr.rend(); ++iter, pow16 *= 16) {
    decimal += hexToDecimal[*iter] * pow16;
  }
  return std::to_string(decimal);
}

char strHex[32] = "Hex";
char hexBoxTextRaw[9] = "";
std::string hexBoxTextCleaned = "";
bool hexBoxEditMode = true;

char strDec[32] = "Decimal";
char decBoxTextRaw[9] = "";
std::string decBoxTextCleaned = "";
bool decBoxEditMode = false;

char strBin[32] = "Binary";
char binBoxTextRaw[65] = "";
std::string binBoxTextCleaned = "";
bool binBoxEditMode = false;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Hex Tool");

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    std::string hexStrRaw{hexBoxTextRaw};
    if (hexStrRaw != hexBoxTextCleaned) {
      // Something has been typed into the hex input, convert it to uppercase then filter out any bad characters
      std::transform(hexStrRaw.begin(), hexStrRaw.end(), hexStrRaw.begin(), std::toupper);
      hexStrRaw.erase(
          std::remove_if(hexStrRaw.begin(), hexStrRaw.end(), [](auto const& c) { return !hexChars.contains(c); }));
      // Update the value of the text boxes to reflect the cleaned value
      memset(hexBoxTextRaw, 0, 9);
      hexBoxTextCleaned = hexStrRaw;
      strncpy(hexBoxTextRaw, hexBoxTextCleaned.c_str(), hexBoxTextCleaned.size());
      // Parse this string to an integer and convert it to decimal and binary
      std::string decimalStr = hexStrToDecimalStr(hexBoxTextCleaned);
      memset(decBoxTextRaw, 0, 9);
      strncpy(decBoxTextRaw, decimalStr.c_str(), decimalStr.size());
      std::string binaryStr = hexStrToBinaryStr(hexBoxTextCleaned);
      memset(binBoxTextRaw, 0, 65);
      strncpy(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    GuiSetStyle(LABEL, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({20, 0, 280, 80}, strHex);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox({20, 60, 280, 80}, hexBoxTextRaw, 9, hexBoxEditMode);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({380, 0, 240, 80}, strDec);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox({380, 60, 240, 80}, decBoxTextRaw, 9, decBoxEditMode);

    GuiLabel({20, 400, 280, 80}, binBoxTextRaw);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();  // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}