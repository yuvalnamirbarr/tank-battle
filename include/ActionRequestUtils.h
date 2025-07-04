#pragma once

#include <string>
#include "ActionRequest.h" 

/**
 * @brief Converts an ActionRequest enum to its string representation.
 * 
 * This function maps each possible ActionRequest value to a human-readable string.
 * It is useful for debugging, logging, and textual output.
 * 
 * @param action The ActionRequest value to convert.
 * @return std::string A string representation of the given action.
 */
inline std::string toString(ActionRequest action) {
    switch (action) {
        case ActionRequest::MoveForward:     return "MoveForward";
        case ActionRequest::MoveBackward:    return "MoveBackward";
        case ActionRequest::RotateLeft90:    return "RotateLeft90";
        case ActionRequest::RotateRight90:   return "RotateRight90";
        case ActionRequest::RotateLeft45:    return "RotateLeft45";
        case ActionRequest::RotateRight45:   return "RotateRight45";
        case ActionRequest::Shoot:           return "Shoot";
        case ActionRequest::GetBattleInfo:   return "GetBattleInfo";
        case ActionRequest::DoNothing:       return "DoNothing";
        default:                             return "Unknown";
    }
}
