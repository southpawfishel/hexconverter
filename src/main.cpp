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

enum {
  DEC_MODE_U8 = 0,
  DEC_MODE_S8,
  DEC_MODE_U16,
  DEC_MODE_S16,
  DEC_MODE_U32,
  DEC_MODE_S32,
  DEC_MODE_U64,
  DEC_MODE_S64,
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

std::string hexStrToDecimalStrForMode(const std::string hexStr, const int mode) {
  switch (mode) {
    case DEC_MODE_U8:
      return hexStrToDecimalStr<uint8_t>(hexStr);
      break;
    case DEC_MODE_S8:
      return hexStrToDecimalStr<int8_t>(hexStr);
      break;
    case DEC_MODE_U16:
      return hexStrToDecimalStr<uint16_t>(hexStr);
      break;
    case DEC_MODE_S16:
      return hexStrToDecimalStr<int16_t>(hexStr);
      break;
    case DEC_MODE_U32:
      return hexStrToDecimalStr<uint32_t>(hexStr);
      break;
    case DEC_MODE_S32:
      return hexStrToDecimalStr<int32_t>(hexStr);
      break;
    case DEC_MODE_U64:
      return hexStrToDecimalStr<uint64_t>(hexStr);
      break;
    case DEC_MODE_S64:
      return hexStrToDecimalStr<int64_t>(hexStr);
      break;
  }
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
    SignedIntType signedDec = (negative ? -1 : 1) * static_cast<SignedIntType>(decimal);
    if (sizeof(IntType) == 1) {
      // Special case for bytes: make sure they are written out as numbers rather than chars
      outputStream << static_cast<uint16_t>(static_cast<int8_t>(signedDec) & 0x00FF);
    } else {
      outputStream << signedDec;
    }
  } else {
    if (sizeof(IntType) == 1) {
      // Special case for bytes: make sure they are written out as numbers rather than chars
      outputStream << static_cast<uint16_t>(decimal);
    } else {
      outputStream << decimal;
    }
  }
  return outputStream.str();
}

std::string decStrToHexStrForMode(const std::string decStr, const int mode) {
  switch (mode) {
    case DEC_MODE_U8:
      return decStrToHexStr<uint8_t>(decStr);
      break;
    case DEC_MODE_S8:
      return decStrToHexStr<int8_t>(decStr);
      break;
    case DEC_MODE_U16:
      return decStrToHexStr<uint16_t>(decStr);
      break;
    case DEC_MODE_S16:
      return decStrToHexStr<int16_t>(decStr);
      break;
    case DEC_MODE_U32:
      return decStrToHexStr<uint32_t>(decStr);
      break;
    case DEC_MODE_S32:
      return decStrToHexStr<int32_t>(decStr);
      break;
    case DEC_MODE_U64:
      return decStrToHexStr<uint64_t>(decStr);
      break;
    case DEC_MODE_S64:
      return decStrToHexStr<int64_t>(decStr);
      break;
  }
}

std::string decStrToBinaryStr(const std::string decStr) { return hexStrToBinaryStr(decStrToHexStr<uint64_t>(decStr)); }

std::string hexLabelStr{"Hex"};
constexpr size_t hexBoxTextLen = 9;
char hexBoxTextRaw[hexBoxTextLen] = "";
bool hexBoxEditMode = true;

std::string decLabelStr{"Decimal"};
constexpr size_t decBoxTextLen = 17;
char decBoxTextRaw[decBoxTextLen] = "";
bool decBoxEditMode = false;

std::string binLabelStr{"Binary"};
constexpr size_t binBoxTextLen = 65;
char binBoxTextRaw[binBoxTextLen] = "";
bool binBoxEditMode = false;

int decMode = DEC_MODE_S8;
bool decModeToggled = false;
bool hexInputChanged = false;
bool decInputChanged = false;
bool binInputChanged = false;

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

  SetTargetFPS(360);
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
        strncpy(hexBoxTextRaw, clipboard.c_str(), clipboard.length());
      } else if (decBoxEditMode && clipboard.length() <= decBoxTextLen - 1) {
        strncpy(decBoxTextRaw, clipboard.c_str(), clipboard.length());
      } else if (binBoxEditMode && clipboard.length() <= binBoxTextLen - 1) {
        strncpy(binBoxTextRaw, clipboard.c_str(), clipboard.length());
      }
    }

    if (decModeToggled) {
      decModeToggled = false;
      std::string hexStrRaw{hexBoxTextRaw};
      if (!hexStrRaw.empty()) {
        // On changing decimal mode, we reinterpret the hex input as the appropriate type
        std::string decimalStr = hexStrToDecimalStrForMode(std::string{hexStrRaw}, decMode);
        memset(decBoxTextRaw, 0, decBoxTextLen);
        strncpy(decBoxTextRaw, decimalStr.c_str(), decimalStr.size());
      }
    }

    if (hexInputChanged) {
      std::string hexStrRaw{hexBoxTextRaw};
      if (!hexStrRaw.empty()) {
        // Something has been typed into the hex input, convert it to uppercase then filter out any bad characters
        std::for_each(hexStrRaw.begin(), hexStrRaw.end(), [](char& c) { c = std::toupper(c); });
        hexStrRaw.erase(
            std::remove_if(hexStrRaw.begin(), hexStrRaw.end(), [](auto const& c) { return !hexChars.contains(c); }));
        // Update the value of the text boxes to reflect the cleaned value
        memset(hexBoxTextRaw, 0, hexBoxTextLen);
        if (!hexStrRaw.empty()) {
          strncpy(hexBoxTextRaw, hexStrRaw.c_str(), hexStrRaw.size());
        }
        // Parse this string to an integer and convert it to decimal and binary
        memset(decBoxTextRaw, 0, decBoxTextLen);
        if (!hexStrRaw.empty()) {
          std::string decimalStr = hexStrToDecimalStrForMode(hexStrRaw, decMode);
          strncpy(decBoxTextRaw, decimalStr.c_str(), decimalStr.size());
        }
        memset(binBoxTextRaw, 0, binBoxTextLen);
        if (!hexStrRaw.empty()) {
          std::string binaryStr = hexStrToBinaryStr(hexStrRaw);
          strncpy(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());
        }
      } else {
        memset(hexBoxTextRaw, 0, hexBoxTextLen);
        memset(decBoxTextRaw, 0, decBoxTextLen);
        memset(binBoxTextRaw, 0, binBoxTextLen);
      }
    }

    if (decInputChanged) {
      std::string decStrRaw{decBoxTextRaw};
      if (decMode == DEC_MODE_U8 || decMode == DEC_MODE_U16 || decMode == DEC_MODE_U32 || decMode == DEC_MODE_U64) {
        if (!decStrRaw.empty()) {
          // Unsigned dec we just remove anything that isn't a decimal digit
          decStrRaw.erase(
              std::remove_if(decStrRaw.begin(), decStrRaw.end(), [](auto const& c) { return !decChars.contains(c); }));
          // Update the value of the text boxes to reflect the cleaned value
          memset(decBoxTextRaw, 0, decBoxTextLen);
          if (!decStrRaw.empty()) {
            strncpy(decBoxTextRaw, decStrRaw.c_str(), decStrRaw.size());
          }
          // Convert this value to decimal and binary
          memset(hexBoxTextRaw, 0, hexBoxTextLen);
          if (!decStrRaw.empty()) {
            std::string hexStr = decStrToHexStrForMode(decStrRaw, decMode);
            strncpy(hexBoxTextRaw, hexStr.c_str(), hexStr.size());
          }
          memset(binBoxTextRaw, 0, binBoxTextLen);
          if (!decStrRaw.empty()) {
            std::string binaryStr = decStrToBinaryStr(decStrRaw);
            strncpy(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());
          }
        } else {
          memset(hexBoxTextRaw, 0, hexBoxTextLen);
          memset(decBoxTextRaw, 0, decBoxTextLen);
          memset(binBoxTextRaw, 0, binBoxTextLen);
        }
      } else {
        bool hasOnlyMinus = (decStrRaw.size() == 1) && (decStrRaw.at(0) == '-');
        if (!decStrRaw.empty() && !hasOnlyMinus) {
          // Something has been typed into the decimal input, filter out bad characters
          auto removeStart = decStrRaw.begin();
          bool hasNegativeSign = false;
          bool numberPortionEmpty = false;
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
          if (decStrRaw.empty() || (decStrRaw.length() == 1 && hasNegativeSign)) {
            numberPortionEmpty = true;
            if (hasNegativeSign) {
              needToReplaceCleanedWithMinus = true;
              strncpy(decBoxTextRaw, "-", 1);
            }
          } else {
            strncpy(decBoxTextRaw, decStrRaw.c_str(), decStrRaw.size());
          }
          // Parse this string to an integer and convert it to hex and binary
          memset(hexBoxTextRaw, 0, hexBoxTextLen);
          if (!numberPortionEmpty) {
            std::string hexStr = decStrToHexStrForMode(decStrRaw, decMode);
            strncpy(hexBoxTextRaw, hexStr.c_str(), hexStr.size());
          }
          memset(binBoxTextRaw, 0, binBoxTextLen);
          if (!numberPortionEmpty) {
            std::string binaryStr = decStrToBinaryStr(decStrRaw);
            strncpy(binBoxTextRaw, binaryStr.c_str(), binaryStr.size());
          }
        } else {
          if (!hasOnlyMinus) {
            memset(decBoxTextRaw, 0, decBoxTextLen);
          }
          memset(hexBoxTextRaw, 0, hexBoxTextLen);
          memset(binBoxTextRaw, 0, binBoxTextLen);
        }
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
    std::string hexBoxTextOld{hexBoxTextRaw};
    if (GuiTextBox({16, 60, 280, 80}, hexBoxTextRaw, hexBoxTextLen, hexBoxEditMode)) {
      hexBoxEditMode = !hexBoxEditMode;
      if (hexBoxEditMode) {
        decBoxEditMode = false;
      }
    }
    hexInputChanged = std::string{hexBoxTextRaw} != hexBoxTextOld;
    if (GuiButton({296, 60, 80, 80}, "#18#")) {
      // Copy via button
      SetClipboardText(hexBoxTextRaw);
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({16, 160, 280, 80}, decLabelStr.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    std::string decBoxTextOld{decBoxTextRaw};
    // GuiTextBox({16, 220, 280, 80}, decBoxTextRaw, decBoxTextLen, decBoxEditMode);
    if (GuiTextBox({16, 220, 280, 80}, decBoxTextRaw, decBoxTextLen, decBoxEditMode)) {
      decBoxEditMode = !decBoxEditMode;
      if (decBoxEditMode) {
        hexBoxEditMode = false;
      }
    }
    decInputChanged = std::string{decBoxTextRaw} != decBoxTextOld;
    if (GuiButton({296, 220, 80, 80}, "#18#")) {
      // Copy via button
      SetClipboardText(decBoxTextRaw);
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    int newMode = decMode;
    GuiToggleGroup({420, 50, 60, 30}, "U8\nS8\nU16\nS16\nU32\nS32\nU64\nS64", &newMode);
    if (newMode != decMode) {
      decModeToggled = true;
      decMode = newMode;
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
    GuiLabel({16, 320, 240, 80}, binLabelStr.c_str());
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    std::string binBoxTextOld{binBoxTextRaw};
    GuiTextBox({16, 380, 480, 80}, binBoxTextRaw, binBoxTextLen, binBoxEditMode);
    binInputChanged = std::string{binBoxTextRaw} != binBoxTextOld;
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