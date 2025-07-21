#include "HelpCommand.h"
#include "../../../Client.h"
#include "../CommandManager.h"

HelpCommand::HelpCommand() : CommandBase("help", "Shows all available commands", "[command]", { "h", "?" }) {
}

bool HelpCommand::execute(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        // Show all commands
        Client::DisplayClientMessage("%s--- Wolfer Commands ---", MCTF::GOLD);
        
        for (auto& command : CommandManager::commandList) {
            std::string aliasStr = "";
            if (!command->aliases.empty()) {
                aliasStr = " (";
                for (size_t i = 0; i < command->aliases.size(); i++) {
                    aliasStr += command->aliases[i];
                    if (i < command->aliases.size() - 1) aliasStr += ", ";
                }
                aliasStr += ")";
            }
            
            Client::DisplayClientMessage("%s.%s%s%s - %s%s", 
                MCTF::GRAY, command->name.c_str(), aliasStr.c_str(), MCTF::WHITE, 
                MCTF::GRAY, command->description.c_str());
        }
        
        Client::DisplayClientMessage("%sUse %s.help <command>%s for detailed usage", 
            MCTF::YELLOW, MCTF::WHITE, MCTF::YELLOW);
        
        return true;
    }
    else if (args.size() == 2) {
        // Show specific command help
        std::string cmdName = args[1];
        std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::tolower);
        
        for (auto& command : CommandManager::commandList) {
            std::vector<std::string> checkList;
            checkList.push_back(command->name);
            for (const std::string& alias : command->aliases)
                checkList.push_back(alias);
            
            for (std::string check : checkList) {
                std::transform(check.begin(), check.end(), check.begin(), ::tolower);
                if (cmdName == check) {
                    Client::DisplayClientMessage("%s--- %s Command Help ---", MCTF::GOLD, command->name.c_str());
                    Client::DisplayClientMessage("%sName: %s%s", MCTF::GRAY, MCTF::WHITE, command->name.c_str());
                    Client::DisplayClientMessage("%sDescription: %s%s", MCTF::GRAY, MCTF::WHITE, command->description.c_str());
                    Client::DisplayClientMessage("%sUsage: %s.%s %s", MCTF::GRAY, MCTF::WHITE, command->name.c_str(), command->usage.c_str());
                    
                    if (!command->aliases.empty()) {
                        std::string aliasStr = "";
                        for (size_t i = 0; i < command->aliases.size(); i++) {
                            aliasStr += command->aliases[i];
                            if (i < command->aliases.size() - 1) aliasStr += ", ";
                        }
                        Client::DisplayClientMessage("%sAliases: %s%s", MCTF::GRAY, MCTF::WHITE, aliasStr.c_str());
                    }
                    
                    return true;
                }
            }
        }
        
        Client::DisplayClientMessage("%sCommand '%s%s%s' not found!", MCTF::RED, MCTF::WHITE, cmdName.c_str(), MCTF::RED);
        return true;
    }
    
    return false;
}