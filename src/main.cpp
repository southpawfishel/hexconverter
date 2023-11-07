#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const std::unordered_set<char> hexChars{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
const std::unordered_set<char> decChars{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const std::unordered_map<char, std::string> hexToBinary{
    {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"},
    {'6', "0110"}, {'7', "0111"}, {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
    {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
};
const std::unordered_map<char, int> hexToDecimal{
    {'0', 0}, {'1', 1}, {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},  {'6', 6},  {'7', 7},
    {'8', 8}, {'9', 9}, {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
};
const std::unordered_map<char, int> charToDecDigit{
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
};

std::string hexStrToBinaryStr(const std::string hexStr) {
  if (hexStr.empty()) {
    return 0;
  }

  std::string output;
  std::for_each(hexStr.begin(), hexStr.end(), [&output](auto const& c) { output.append(hexToBinary.at(c)); });
  return output;
}

template <typename IntType>
std::string hexStrToDecimalStr(const std::string hexStr) {
  if (hexStr.empty()) {
    return 0;
  }

  IntType decimal = 0;
  int pow16 = 1;
  for (auto iter = hexStr.rbegin(); iter != hexStr.rend(); ++iter, pow16 *= 16) {
    decimal += hexToDecimal.at(*iter) * pow16;
  }
  return std::to_string(decimal);
}

template <typename IntType>
std::string decStrToHexStr(std::string decStr) {
  if (decStr.empty()) {
    return 0;
  }

  using UnsignedIntType = std::make_unsigned_t<IntType>;
  using SignedIntType = std::make_signed_t<IntType>;

  std::ostringstream outputStream;
  outputStream << std::hex << std::uppercase;

  // Check if our value is negative (by definition its signed)
  // Remember that it's negative and delete the leading minus.
  bool negative = false;
  if (decStr.starts_with("-")) {
    negative = true;
    decStr.erase(0, 1);
  }

  // Loop over our string and convert to decimal
  UnsignedIntType decimal = 0;
  int pow10 = 1;
  for (auto iter = decStr.rbegin(); iter != decStr.rend(); ++iter, pow10 *= 10) {
    decimal += charToDecDigit.at(*iter) * pow10;
  }

  if (std::is_signed<IntType>::value) {
    SignedIntType signedDec = -1 * static_cast<SignedIntType>(decimal);
    outputStream << signedDec;
  } else {
    outputStream << decimal;
  }
  return outputStream.str();
}

template <typename IntType>
std::string decStrToBinaryStr(const std::string decStr) {
  return hexStrToBinaryStr(decStrToHexStr<IntType>(decStr));
}

std::string hexLabelStr{"Hex"};
constexpr size_t hexBoxTextLen = 9;
char hexBoxTextRaw[hexBoxTextLen] = "";
std::string hexBoxTextCleaned = "";
bool hexBoxEditMode = true;

std::string decLabelStr{"Decimal"};
constexpr size_t decBoxTextLen = 17;
char decBoxTextRaw[decBoxTextLen] = "";
std::string decBoxTextCleaned = "";
bool decBoxEditMode = false;

std::string binLabelStr{"Binary"};
constexpr size_t binBoxTextLen = 65;
char binBoxTextRaw[binBoxTextLen] = "";
std::string binBoxTextCleaned = "";
bool binBoxEditMode = false;

int toggleGroup = 0;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 600;
  const int screenHeight = 512;

  InitWindow(screenWidth, screenHeight, "Hex Converter");
  GuiSetIconScale(2);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    if (ctrl && IsKeyPressed(KEY_C)) {
      // Handling copy
      if (hexBoxEditMode) {
        SetClipboardText(hexBoxTextRaw);
      } else if (decBoxEditMode) {
        SetClipboardText(decBoxTextRaw);
      } else if (binBoxEditMode) {
        SetClipboardText(binBoxTextRaw);
      }
    } else if (ctrl && IsKeyPressed(KEY_V)) {
      // Handling paste
      std::string clipboard{GetClipboardText()};
      if (hexBoxEditMode && clipboard.length() <= hexBoxTextLen - 1) {
        if (clipboard.starts_with("0x")) {
          clipboard.erase(0, 2);
        }
        strncpy_s(hexBoxTextRaw, clipboard.c_str(), clipboard.length());
      } else if (decBoxEditMode && clipboard.length() <= decBoxTextLen - 1) {
        strncpy_s(decBoxTextRaw, clipboard.c_str(), clipboard.length());
      } else if (binBoxEditMode && clipboard.length() <= binBoxTextLen - 1) {
        strncpy_s(binBoxTextRaw, clipboard.c_str(), clipboard.length());
      }
    }

    std::string hexStrRaw{hexBoxTextRaw};
    if (!hexStrRaw.empty() && hexStrRaw != hexBoxTextCleaned) {
      // Something has been typed into the hex input, convert it to uppercase then filter out any bad characters
      std::transform(hexStrRaw.begin(), hexStrRaw.end(), hexStrRaw.begin(), std::toupper);
      hexStrRaw.erase(
          std::remove_if(hexStrRaw.begin(), hexStrRaw.end(), [](auto const& c) { return !hexChars.contains(c); }));
      // Update the value of the text boxes to reflect the cleaned value
      memset(hexBoxTextRaw, 0, hexBoxTextLen);
      hexBoxTextCleaned = hexStrRaw;
      strncpy_s(hexBoxTextRaw, hexBoxTextCleaned.c_str(), hexBoxTextCleaned.size());
      // Parse this string to an integer and convert it to decimal and binary
      std::string decimalStr = hexStrToDecimalStr<uint32_t>(hexBoxTextCleaned);
      memset(decBoxTextRaw, 0, decBoxTextLen);
      strncpy_s(decBoxTextRaw, decimalStr.c_str(), decimalStr.size());
      std::string binaryStr = hexStrToBinaryStr(hexBoxTextCleaned);
      memset(binBoxTextRaw, 0, binBoxTextLen);
      strncpy_s(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());
    }

    std::string decStrRaw{decBoxTextRaw};
    bool hasOnlyMinus = (decStrRaw.size() == 1) && (decStrRaw.at(0) == '-');
    if (!decStrRaw.empty() && !hasOnlyMinus && decStrRaw != decBoxTextCleaned) {
      // Something has been typed into the decimal input, filter out bad characters
      auto removeStart = decStrRaw.begin();
      bool hasNegativeSign = false;
      bool needToReplaceCleanedWithMinus = false;
      if (!decStrRaw.empty() && !decChars.contains(decStrRaw.at(0))) {
        // Front of string doesn't have a number, remove it unless its a minus sign
        if (!(decStrRaw.at(0) == '-')) {
          decStrRaw.erase(0, 1);
        } else {
          hasNegativeSign = true;
          ++removeStart;
        }
      }
      decStrRaw.erase(
          std::remove_if(removeStart, decStrRaw.end(), [](auto const& c) { return !decChars.contains(c); }));
      // Update the value of the text boxes to reflect the cleaned value
      memset(decBoxTextRaw, 0, decBoxTextLen);
      decBoxTextCleaned = decStrRaw;
      if (decBoxTextCleaned.empty()) {
        if (!hasNegativeSign) {
          decBoxTextCleaned = "0";
          strncpy_s(decBoxTextRaw, decBoxTextCleaned.c_str(), decBoxTextCleaned.size());
        } else {
          needToReplaceCleanedWithMinus = true;
          decBoxTextCleaned = "0";
          strncpy_s(decBoxTextRaw, "-", 1);
        }
      } else {
        strncpy_s(decBoxTextRaw, decBoxTextCleaned.c_str(), decBoxTextCleaned.size());
      }
      // Parse this string to an integer and convert it to hex and binary
      std::string hexStr = decStrToHexStr<uint32_t>(decBoxTextCleaned);
      memset(hexBoxTextRaw, 0, hexBoxTextLen);
      strncpy_s(hexBoxTextRaw, hexStr.c_str(), hexStr.size());
      std::string binaryStr = decStrToBinaryStr<uint32_t>(decBoxTextCleaned);
      memset(binBoxTextRaw, 0, binBoxTextLen);
      strncpy_s(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());

      if (needToReplaceCleanedWithMinus) {
        decBoxTextCleaned = "-";
      }
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    GuiSetStyle(LABEL, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({16, 0, 280, 80}, hexLabelStr.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    if (GuiTextBox({16, 60, 280, 80}, hexBoxTextRaw, hexBoxTextLen, hexBoxEditMode)) {
      hexBoxEditMode = !hexBoxEditMode;
      if (hexBoxEditMode) {
        decBoxEditMode = false;
      }
    }
    if (GuiButton({296, 60, 80, 80}, "#18#")) {
      // Copy via button
      SetClipboardText(hexBoxTextRaw);
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({16, 160, 280, 80}, decLabelStr.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox({16, 220, 280, 80}, decBoxTextRaw, decBoxTextLen, decBoxEditMode);
    // if (GuiTextBox({380, 60, 280, 80}, decBoxTextRaw, decBoxTextLen, decBoxEditMode)){
    //   decBoxEditMode = !decBoxEditMode;
    //   if (decBoxEditMode) {
    //     hexBoxEditMode = false;
    //   }
    // }
    if (GuiButton({296, 220, 80, 80}, "#18#")) {
      // Copy via button
      SetClipboardText(decBoxTextRaw);
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiToggleGroup({420, 50, 60, 30}, "U8\nS8\nU16\nS16\nU32\nS32\nU64\nS64", &toggleGroup);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({16, 320, 240, 80}, binLabelStr.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox({16, 380, 480, 80}, binBoxTextRaw, binBoxTextLen, binBoxEditMode);
    if (GuiButton({496, 380, 80, 80}, "#18#")) {
      // Copy via button
      SetClipboardText(binBoxTextRaw);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();  // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}