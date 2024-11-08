# ---------------------------
# CENTURION
# [2019] - [2022] Rattlesmake
# All Rights Reserved.
# ---------------------------

# Script for Editor_ui.xml

#### GLOBAL FUNCTIONS

def hello_marjo(args):
    print("hello, marjo!")

def close_iframe(args):
    args['iframe'].Close()

def save_adventure(args):
    args['adventure'].Save()

def open(args, name):
    args['iframes'][name].Open()    

def is_adventure_opened(args):
    return args['adventure'].IsOpened()
    
def is_adventure_edited(args):
    return args['adventure'].IsEdited()

def is_iframe_opened(args, name):
    return args['iframes'][name].IsOpened()

def set_button_status(args, buttonId, status):
    button_id = 'button#' + str(buttonId)
    args['iframe'][button_id].enabled = status

def set_button_status_text_input(args, textInputId, buttonId):
    if (args['iframe']['textInput#'+str(textInputId)].text == ""):
        set_button_status(args, buttonId, False)
    else:
        set_button_status(args, buttonId, True)



# ---------------------
##### EDITOR MENU #####
# ---------------------

##### EDITOR #####
    
def edmenu_quit_click(args): # quit from editor
    args['editor'].Quit()

def edmenu_saveadv_click(args): # save adventure
    save_adventure(args)

##### TOOLS #####

def edmenu_runadventure_click(args): # run adventure
    args['editor'].RunAdventure()

def edmenu_objprops_condition(args): # obj properties condition
    return args['editor'].CheckOpenObjectPropertiesCondition()

def edmenu_objprops_click(args):
    args['editor'].OpenObjectProperties()

def edmenu_editormenu_condition(args):
    newAdvClosed = not is_iframe_opened(args, 'newAdv') 
    openAdvClosed = not is_iframe_opened(args, 'openAdv')
    saveAdvAsClosed = not is_iframe_opened(args, 'SaveasAdv')
    saveAdvCopyClosed = not is_iframe_opened(args, 'SaveCopyAdv')
    deleteAdvClosed = not is_iframe_opened(args, 'deleteAdv')
    return newAdvClosed and openAdvClosed and saveAdvAsClosed and saveAdvCopyClosed and deleteAdvClosed

def edmenu_adventuremenu_condition(args):
    advOpened = is_adventure_opened(args)
    newAdvClosed = not is_iframe_opened(args, 'newAdv') 
    openAdvClosed = not is_iframe_opened(args, 'openAdv')
    saveAdvAsClosed = not is_iframe_opened(args, 'SaveasAdv')
    saveAdvCopyClosed = not is_iframe_opened(args, 'SaveCopyAdv')
    deleteAdvClosed = not is_iframe_opened(args, 'deleteAdv')
    return advOpened and newAdvClosed and openAdvClosed and saveAdvAsClosed and saveAdvCopyClosed and deleteAdvClosed

def edmenu_toolsmenu_condition(args):
    advOpened = is_adventure_opened(args)
    newAdvClosed = not is_iframe_opened(args, 'newAdv') 
    openAdvClosed = not is_iframe_opened(args, 'openAdv')
    saveAdvAsClosed = not is_iframe_opened(args, 'SaveasAdv')
    saveAdvCopyClosed = not is_iframe_opened(args, 'SaveCopyAdv')
    deleteAdvClosed = not is_iframe_opened(args, 'deleteAdv')
    return advOpened and newAdvClosed and openAdvClosed and saveAdvAsClosed and saveAdvCopyClosed and deleteAdvClosed

##### DEBUG #####

def edmenu_togglegrid(args):
    args['scenario'].ToggleGrid()

def edmenu_togglehitboxes(args):
    args['scenario'].ToggleHitboxes()
    
def edmenu_togglewireframe(args):
    args['scenario'].ToggleWireframe()

def edmenu_togglesea(args):
    args['scenario'].ToggleSea()

def edmenu_toggleclouds(args):
    args['scenario'].ToggleClouds()

def edmenu_toggletracingdebugging(args):
    args['scenario'].ToggleTracingDebugging()
    
def edmenu_toggledrawingnoise(args):
    args['scenario'].ToggleDrawWithoutNoise()    

# ------------------------
##### EDITOR WINDOWS #####
# ------------------------

##### EMPTY WINDOW #####
def edwin_empty_runtime(args):
    status = "Unsaved"
    if is_adventure_opened(args):
        if is_adventure_edited(args):
            status = "Edited"
        else:
            status = "Saved"

    args['iframe']['text#0'].SetText(args['editor_ui'].infoText, False)
    args['iframe']['text#1'].SetText("Adventure status: " + status, False)
    args['iframe']['text#2'].SetText("Adventure name: " + args['adventure'].name, False)
    args['iframe']['text#3'].SetText("Adventure zip: " + args['adventure'].zipName, False)
    args['iframe']['text#4'].SetText("Scenario folder: " + args['scenario'].folderName, False)
    args['iframe']['text#5'].SetText("Scenario name: " + args['scenario'].name, False)
    args['iframe']['text#6'].SetText("Scenario size: " + args['scenario'].size, False)
    
##### NEW ADVENTURE #####
def edwin_newadv_opening(args):
    set_button_status(args, 0, False)
    args['iframe']['textInput#0'].Reset()

def edwin_newadv_runtime(args):
    set_button_status_text_input(args, 0, 0)
    
def edwin_newadv_button0(args):
    args['editor'].NewAdventure(args['iframe']['textInput#0'].text)
    close_iframe(args)

##### OPEN ADVENTURE #####
def edwin_openadv_opening(args):
    advsDetailList = args['engine'].GetListOfValidAdventuresWithDetails()
    set_button_status(args, 0, False)
    args['iframe']['textList#0'].UpdateOptions(advsDetailList, '', '')
    
def edwin_openadv_txtlst0(args):
    set_button_status(args, 0, True)

def edwin_openadv_button0(args):
    idx = args['iframe']['textList#0'].selectedIndex
    advList = args['engine'].GetListOfValidAdventures()
    advName = advList[idx]
    args['editor'].OpenAdventure(advName)
    close_iframe(args)

##### DELETE ADVENTURE #####
def edwin_deleteadv_opening(args):
    advsDetailList = args['engine'].GetListOfValidAdventuresWithDetails()
    set_button_status(args, 0, False)
    args['iframe']['textList#0'].UpdateOptions(advsDetailList, '', '')
    
def edwin_deleteadv_txtlst0(args):
    set_button_status(args, 0, True)

def edwin_deleteadv_button0(args):
    idx = args['iframe']['textList#0'].selectedIndex
    advList = args['engine'].GetListOfValidAdventures()
    advName = advList[idx]
    args['editor'].DeleteAdventure(advName)
    close_iframe(args)

##### SAVE ADVENTURE AS #####
def edwin_saveadvas_opening(args):
    args['iframe']['textInput#0'].SetPlaceholder(args['adventure'].name, False)

def edwin_saveadvas_runtime(args):
    set_button_status_text_input(args, 0, 0)

def edwin_saveadvas_button0(args):
    advName = args['iframe']['textInput#0'].text
    args['editor'].SaveAdventureAs(advName)
    close_iframe(args)

##### SAVE ADVENTURE COPY #####
def edwin_saveadvcopy_opening(args):
    args['iframe']['textInput#0'].SetPlaceholder(args['adventure'].name, False)

def edwin_saveadvcopy_runtime(args):
    set_button_status_text_input(args, 0, 0)

def edwin_saveadvcopy_button0(args):
    advName = args['iframe']['textInput#0'].text
    args['editor'].SaveAdventureCopy(advName)
    close_iframe(args)

##### NEW SCENARIO #####
def edwin_newscnr_opening(args):
    set_button_status(args, 0, False)
    args['iframe']['textInput#0'].Reset()

def edwin_newscrn_runtime(args):
    set_button_status_text_input(args, 0, 0)

def edwin_newscrn_button0(args):
    args['adventure'].NewScenario(args['iframe']['textInput#0'].text)
    close_iframe(args)

##### OPEN SCENARIO #####
def edwin_openscnr_opening(args):
    scenarioList = args['adventure'].GetListOfValidScenariosWithDetails()
    set_button_status(args, 0, False)
    args['iframe']['textList#0'].UpdateOptions(scenarioList, '', '')
    
def edwin_openscnr_txtlst0(args):
    set_button_status(args, 0, True)

def edwin_openscnr_button0(args):
    idx = args['iframe']['textList#0'].selectedIndex
    name = args['adventure'].GetListOfValidScenarios()[idx]
    args['adventure'].OpenScenario(name)
    close_iframe(args)

##### DELETE SCENARIO #####
def edwin_deletescnr_opening(args):
    scenarioList = args['adventure'].GetListOfValidScenariosWithDetails()
    set_button_status(args, 0, False)
    args['iframe']['textList#0'].UpdateOptions(scenarioList, '', '')
    
def edwin_deletescnr_txtlst0(args):
    set_button_status(args, 0, True)

def edwin_deletescnr_button0(args):
    idx = args['iframe']['textList#0'].selectedIndex
    name = args['adventure'].GetListOfValidScenarios()[idx]
    args['adventure'].DeleteScenario(name)
    close_iframe(args)

##### IMPORT SCENARIO #####

# THIS SECTION MUST BE REWROTE FROM SCRATCH.  SKIPPED.

##### EXPORT SCENARIO #####

# THIS SECTION MUST BE REWROTE FROM SCRATCH.  SKIPPED.

##### ADVENTURE SCRIPTS #####
def edwin_advscripts_opening(args):
    args['iframe']['textInputMultiline#0'].Reset()
   
def edwin_advscripts_tab1_opening(args):
    thisTab = args['tab']
    thisTab['image#0'].SetImage("common/iframe/" + args['iframe'].GetSkinName() + "/status")
    thisTab['image#1'].SetImage("common/iframe/" + args['iframe'].GetSkinName() + "/status")
    thisTab['text#0'].SetText("e_text_waiting_for_script", True)

def edwin_advscripts_tab1_runtime(args):
    thisTab = args['tab']
    lin = str(thisTab['textInputMultiline#0'].line)
    col = str(thisTab['textInputMultiline#0'].column)
    thisTab['text#1'].SetText(lin +  ", " + col, False)

def edwin_advscripts_tab1_button0(args):
    #local thisTab = this:GetTabByIndex(1)
    #local _luaScript = thisTab:GetTextInputMultilineById(0)
    #thisTab:UpdateTextById(0, Compile(_luaScript))
    print("edwin_advscripts_tab1_button0")

def edwin_advscripts_tab2_opening(args):
    thisTab = args['tab']
    thisTab['image#0'].SetImage("common/iframe/" + args['iframe'].GetSkinName() + "/status")
    thisTab['image#1'].SetImage("common/iframe/" + args['iframe'].GetSkinName() + "/status")
    thisTab['text#0'].SetText("e_text_waiting_for_script", True)

def edwin_advscripts_tab2_runtime(args):
    thisTab = args['tab']
    thisTab['text#1'].SetText(str(thisTab.GetTextInputMultilineCursorLineById(0)) +  ", " + str(thisTab.GetTextInputMultilineCursorColumnById(0)), False)

def edwin_advscripts_tab2_button0(args):
    #local thisTab = this:GetTabByIndex(2)
    #local _luaScript = thisTab:GetTextInputMultilineById(0)
    #thisTab:UpdateTextById(0, Compile(_luaScript))
    print("edwin_advscripts_tab2_button0")

##### ADVENTURE OBJECTIVES #####
def edwin_advobjectives_opening(args):
    #Reset user interface interaction
        
    args['iframe']['text#3'].SetText("", False)
    #args['iframe'].EmptyComboBoxById(0) ?
    #args['iframe'].EmptyComboBoxById(1) ? 
    args['iframe']['textInput#0'].Reset()
    args['iframe']['textInput#1'].Reset()
    args['iframe']['textInputMultiline#0'].Reset()
    args['iframe']['checkBox#0'].checked = False
    args['iframe']['textInput#0'].enabled = False
    args['iframe']['textInput#1'].enabled = False
    args['iframe']['textInputMultiline#0'].enabled = False
    args['iframe']['checkBox#0'].enabled = False
    args['iframe']['button#1'].enabled = False
    args['iframe']['button#3'].enabled = False
	
    #Update objectives list
    #local _objectivesList = Editor.GetAdventureObjectives()
    #this:UpdateTextListById(0, _objectivesList)
    print("edwin_advobjectives_opening")
    
def edwin_advobjectives_runtime(args):
    #this:UpdateTextListById(0, Editor.GetAdventureObjectives())
    #local insertingID = this:GetTextInputById(0)
    #local isIdOk = Editor.IsObjectiveIdAvailable(insertingID, currentObjective)

    #if (isIdOk == false) then
    #    this:UpdateTextById(3, "E_Text_UniqueID", true)
    #elseif (insertingID == "" and currentObjective ~= nil) then
    #    this:UpdateTextById(3, "E_Text_NoBlankID", true)
    #elseif (currentObjective ~= nil) then
    #    this:UpdateTextById(3, "")
    #    currentObjective.id = insertingID
    #    currentObjective.title = this:GetTextInputById(1)
    #    currentObjective.description = this:GetTextInputMultilineById(0)
    #    currentObjective.scenario = this:GetComboBoxSelectedById(0)
    #    currentObjective.minimap = this:IsCheckBoxSelectedById(0)
    #end
    x = ""
    
def edwin_advobjectives_closing(args):
    #currentObjective = nil
    print("edwin_advobjectives_closing")
    
def edwin_advobjectives_txtInpt0(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_txtInpt0")
    
def edwin_advobjectives_txtInpt1(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_txtInpt1")
    
def edwin_advobjectives_txtInptMlt0(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_txtInptMlt0")
    
def edwin_advobjectives_cmbbx0(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_cmbbx0")
    
def edwin_advobjectives_cmbbx1(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_cmbbx1")
    
def edwin_advobjectives_chckbx0(args):
    #if currentObjective ~= nil then
    #Editor.MarkAsEdited()
    #end
    print("edwin_advobjectives_chckbx0")
    
def edwin_advobjectives_txtlst0(args):
    #local objectiveID = this:GetTextListSelectedById(0)
    #currentObjective = Editor.GetAdventureObjectiveById(objectiveID)
    #-- Load user interface inputs
    #local _availableScenarios = GetAllValidScenarios()
    #this:UpdateComboBoxById(0, _availableScenarios, "" , Editor.GetAdventureDefaultScenario())
    #this:UpdateComboBoxById(1, _availableScenarios, "" , Editor.GetAdventureDefaultScenario())
    #this:SetTextInputTextById(0, currentObjective.id)
    #this:SetTextInputTextById(1, currentObjective.title)
    #this:SetTextInputMultilineTextById(0, currentObjective.description)
    #this:SetComboBoxIndexById(0, currentObjective.scenario)
    #this:SetCheckBoxStatusById(0, currentObjective.minimap)
    #-- Enable user interface commands
    #this:EnableTextInputById(0)
    #this:EnableTextInputById(1)
    #this:EnableTextInputMultilineById(0)
    #this:EnableCheckBoxById(0)
    #this:EnableButtonById(1)
    #this:EnableButtonById(3)
    print("edwin_advobjectives_txtlst0")
    
def edwin_advobjectives_button0(args):
    #Editor.NewAdventureObjective()
    #Editor.MarkAsEdited()
    print("edwin_advobjectives_button0")
    
def edwin_advobjectives_button1(args):
    #Editor.DeleteAdventureObjectiveById(currentObjective.id)
    #currentObjective = nil
    #this:UpdateTextById(3, "")
    #this:EmptyComboBoxById(0)
    #this:EmptyComboBoxById(1)
    #this:ResetTextInputById(0)
    #this:ResetTextInputById(1)
    #this:ResetTextInputMultilineById(0)
    #this:SetCheckBoxStatusById(0, false)
    print("edwin_advobjectives_button1")

##### ADVENTURE CONVERSATIONS #####

# THIS SECTION MUST BE REWROTE FROM SCRATCH.  SKIPPED.

##### ADVENTURE PROPERTIES #####
def edwin_advprops_opening(args):
    #Initialize tab0
    tab0 = args['iframe'].GetTabByIndex(0)
    
    availableScenarios = args['adventure'].GetListOfValidScenarios()
    tab0['textInput#0'].SetPlaceholder(args['adventure'].name, False)
    tab0['textInput#1'].SetPlaceholder(args['adventure'].author, False)
    tab0['textInputMultiline#0'].SetPlaceholder(args['adventure'].description, False)
    tab0['comboBox#0'].UpdateOptions(availableScenarios, "" , args['adventure'].startingScenario)
    tab0['inputInt#0'].value = args['adventure'].playersNumber
    #local _gameModes = GetListOfGameModes()
    #_tab0:UpdateComboBoxById(1, _gameModes, "W_" , "")

def edwin_advprops_button0(args):
    #Tab no. 0
    tab0 = args['iframe'].GetTabByIndex(0)

    args['adventure'].name = tab0['textInput#0'].text
    args['adventure'].author = tab0['textInput#1'].text
    args['adventure'].description = tab0['textInputMultiline#0'].text
    args['adventure'].startingScenario = tab0['comboBox#0'].selectedText
    args['adventure'].MarkAsEdited()
    close_iframe(args)

##### PLAYERS PROPERTIES #####
def edwin_plyrsprops_opening(args):
    #Initialize tab0
    tab0 = args['iframe'].GetTabByIndex(0)
    listOfRaces = args['engine'].GetAllRacesNames()
    maxPlayers = args['players_settings'].maxAllowedPlayers
    #difficulties = GetListOfDifficulties()

    for i in range (1, maxPlayers + 1):
        #Need to update color according to player's one
        tab0['textInput#'+str(i-1)].SetPlaceholder(args['players'][i].name, True)
        tab0['comboBox#'+str(i-1)].UpdateOptions(listOfRaces, "r_", args['players'][i].race)
		#Need to update player difficulty 0 - NO AI; 1 - EASY; 2 - NORMAL; 3 - HARD

    #Initialize tab1
    tab1 = args['iframe'].GetTabByIndex(1)
    playersNameList = []

    #Update players texts
    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)

    #Update players list according to the players number
    tab1['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[0])
    
    #for i in range(7):
    #    for j in range(4):
            #tab1.AddButton(i * 4 + j, 288 + 34 * j, 104 + 32 * i, 32, 32, "", "", "")

    #Update diplomacy relationships buttons
    # -------------- UNCOMMENT THIS SECTION WHEN DIPLOMACY IS IMPLEMENTED ---------------
    #currentPlayer = tab1.GetComboBoxIndexById(0) + 1
    #rows = 0
    #value = 0
    #cmd = ""
    #imageNames = [
    #["editor/dipl_no_ceaseFire", "editor/dipl_ceaseFire"],
    #["editor/dipl_no_shareSupport", "editor/dipl_shareSupport"],
    #["editor/dipl_no_shareView", "editor/dipl_shareView"],
    #["editor/dipl_no_shareControl", "editor/dipl_shareControl"]
    #]

    #for i in range(1, 9):
    #    if (j != currentPlayer):
    #        value = 1
    #        if (Diplomacy.GetCeaseFire(i, j) == true):
    #            value = 2
    #       tab1.UpdateButtonImageById(rows * 4, imageNames[0][value])
    #        cmd =       "if (Diplomacy.GetCeaseFire(" + str(currentPlayer) + ", " + str(j) + ") == false):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1).UpdateButtonImageById(" + str(rows * 4) + ", \"" + imageNames[0][2] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetCeaseFire(" + str(currentPlayer) + ", " + str(j) + ", true)\n"
    #        cmd = cmd + "elif (Diplomacy.GetCeaseFire(" + str(currentPlayer) + ", " + str(j) + ") == true):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1):UpdateButtonImageById(" + str(rows * 4) +  ", \"" + imageNames[0][1] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetCeaseFire(" + str(currentPlayer) + ", " + str(j) + ", false)\n"
    #        cmd = cmd + "args['adventure].MarkAsEdited()"
    #        tab1.UpdateButtonCommandById(rows * 4, cmd)
    #
    #        value = 1
    #        if (Diplomacy.GetShareSupport(i, j) == true):
    #            value = 2
    #        tab1.UpdateButtonImageById(rows * 4, imageNames[1][value])
    #        cmd =       "if (Diplomacy.GetShareSupport(" + str(currentPlayer) + ", " + str(j) + ") == false):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1).UpdateButtonImageById(" + str(rows * 4) + ", \"" + imageNames[1][2] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareSupport(" + str(currentPlayer) + ", " + str(j) + ", true)\n"
    #        cmd = cmd + "elif (Diplomacy.GetShareSupport(" + str(currentPlayer) + ", " + str(j) + ") == true):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1):UpdateButtonImageById(" + str(rows * 4) +  ", \"" + imageNames[1][1] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareSupport(" + str(currentPlayer) + ", " + str(j) + ", false)\n"
    #        cmd = cmd + "args['adventure].MarkAsEdited()"
    #        tab1.UpdateButtonCommandById(rows * 4, cmd)
    #
    #        value = 1
    #        if (Diplomacy.GetShareView(i, j) == true):
    #            value = 2
    #        tab1.UpdateButtonImageById(rows * 4, imageNames[2][value])
    #        cmd =       "if (Diplomacy.GetShareView(" + str(currentPlayer) + ", " + str(j) + ") == false):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1).UpdateButtonImageById(" + str(rows * 4) + ", \"" + imageNames[2][2] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareView(" + str(currentPlayer) + ", " + str(j) + ", true)\n"
    #        cmd = cmd + "elif (Diplomacy.GetShareView(" + str(currentPlayer) + ", " + str(j) + ") == true):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1):UpdateButtonImageById(" + str(rows * 4) +  ", \"" + imageNames[2][1] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareView(" + str(currentPlayer) + ", " + str(j) + ", false)\n"
    #        cmd = cmd + "args['adventure].MarkAsEdited()"
    #        tab1.UpdateButtonCommandById(rows * 4, cmd)
    #
    #        value = 1
    #        if (Diplomacy.GetShareControl(i, j) == true):
    #            value = 2
    #        tab1.UpdateButtonImageById(rows * 4, imageNames[3][value])
    #        cmd =       "if (Diplomacy.GetShareControl(" + str(currentPlayer) + ", " + str(j) + ") == false):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1).UpdateButtonImageById(" + str(rows * 4) + ", \"" + imageNames[3][2] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareControl(" + str(currentPlayer) + ", " + str(j) + ", true)\n"
    #        cmd = cmd + "elif (Diplomacy.GetShareControl(" + str(currentPlayer) + ", " + str(j) + ") == true):\n"
    #        cmd = cmd + "    args['iframe'].GetTabByIndex(1):UpdateButtonImageById(" + str(rows * 4) +  ", \"" + imageNames[3][1] + ")\n"
    #        cmd = cmd + "    Diplomacy.SetShareControl(" + str(currentPlayer) + ", " + str(j) + ", false)\n"
    #        cmd = cmd + "args['adventure].MarkAsEdited()"
    #        tab1.UpdateButtonCommandById(rows * 4, cmd)
    #
    #        rows = rows + 1
    
def edwin_plyrsprops_tab1_cmbbx0(args):
    # local thisTab = this:GetTabByIndex(1)
    # local _playersBox = thisTab:GetComboBoxIndexById(0)

    # -- Update players texts
    # if (_playersBox == 0) then
        # for i = 0, 6 do
            # thisTab:UpdateTextById(i, "W_Player_" ..  i + 2, true)
        # end
    # else
        # for i = 0, 6 do
            # if (i < _playersBox) then
                # thisTab:UpdateTextById(i, "W_Player_" ..  i + 1, true)
            # else
                # thisTab:UpdateTextById(i, "W_Player_" ..  i + 2, true)
            # end
        # end
    # end

    # -- Update diplomacy relationships buttons
    # local _rows = 0
    # local _imageNames = {}
    # _imageNames[0] = {"editor/dipl_no_ceaseFire", "editor/dipl_ceaseFire"}
    # _imageNames[1] = {"editor/dipl_no_shareSupport", "editor/dipl_shareSupport"}
    # _imageNames[2] = {"editor/dipl_no_shareView", "editor/dipl_shareView"}
    # _imageNames[3] = {"editor/dipl_no_shareControl", "editor/dipl_shareControl"}

    # for j = 1, 8 do
        # if (j ~= _playersBox + 1) then
            # local _value = 1
            # if (_playerDipl[_playersBox + 1][j][0] == true) then
                # _value = 2
            # end
            # thisTab:UpdateButtonImageById(_rows * 4, _imageNames[0][_value])
            # local _cmd = "if (_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][0] == false) then"
                  # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..
                  # _rows * 4 ..  ", \"" ..  _imageNames[0][2] ..  "\");"
                  # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j
                  # ..  "][0] = true;"
                  # _cmd = _cmd ..  "\nelseif (_playerDipl[" ..  _playersBox + 1 ..  "]["
                  # ..  j ..  "][0] == true) then"
                  # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..
                  # _rows * 4 ..  ", \"" ..  _imageNames[0][1] ..  "\");"
                  # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j
                  # ..  "][0] = false;\nend"
                  # _cmd = _cmd ..  "\nEditor.MarkAsEdited()"
            # thisTab:UpdateButtonCommandById(_rows * 4, _cmd)
            
            # _value = 1
            # if (_playerDipl[_playersBox + 1][j][1] == true) then
                # _value = 2
            # end
            # thisTab:UpdateButtonImageById(_rows * 4 + 1, _imageNames[1][_value])
            # _cmd = "if (_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..  "][1] ==
            # false) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 1 ..  ", \"" ..  _imageNames[1][2] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][1] = true;"
            # _cmd = _cmd ..  "\nelseif (_playerDipl[" ..  _playersBox + 1 ..  "][" ..
            # j ..  "][1] == true) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 1 ..  ", \"" ..  _imageNames[1][1] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][1] = false;\nend"
            # _cmd = _cmd ..  "\nEditor.MarkAsEdited()"
            # thisTab:UpdateButtonCommandById(_rows * 4 + 1, _cmd)
            
            # _value = 1
            # if (_playerDipl[_playersBox + 1][j][2] == true) then
                # _value = 2
            # end
            # thisTab:UpdateButtonImageById(_rows * 4 + 2, _imageNames[2][_value])
            # _cmd = "if (_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..  "][2] ==
            # false) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 2 ..  ", \"" ..  _imageNames[2][2] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][2] = true;"
            # _cmd = _cmd ..  "\nelseif (_playerDipl[" ..  _playersBox + 1 ..  "][" ..
            # j ..  "][2] == true) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 2 ..  ", \"" ..  _imageNames[2][1] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][2] = false;\nend"
            # _cmd = _cmd ..  "\nEditor.MarkAsEdited()"
            # thisTab:UpdateButtonCommandById(_rows * 4 + 2, _cmd)
        
            # _value = 1
            # if (_playersBox + 1 ~= j and _playerDipl[_playersBox + 1][j][3] == true)
            # then
                # _value = 2
            # end
            # thisTab:UpdateButtonImageById(_rows * 4 + 3, _imageNames[3][_value])
            # _cmd = "if (_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..  "][3] ==
            # false) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 3 ..  ", \"" ..  _imageNames[3][2] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][3] = true;"
            # _cmd = _cmd ..  "\nelseif (_playerDipl[" ..  _playersBox + 1 ..  "][" ..
            # j ..  "][3] == true) then"
            # _cmd = _cmd ..  "\n\tthis:GetTabByIndex(1):UpdateButtonImageById(" ..  _rows
            # * 4 + 3 ..  ", \"" ..  _imageNames[3][1] ..  "\");"
            # _cmd = _cmd ..  "\n\t_playerDipl[" ..  _playersBox + 1 ..  "][" ..  j ..
            # "][3] = false;\nend"
            # _cmd = _cmd ..  "\nEditor.MarkAsEdited()"
            # thisTab:UpdateButtonCommandById(_rows * 4 + 3, _cmd)
            
            # _rows = _rows + 1
        # end
    # end
    print("edwin_plyrsprops_tab1_opening")
    
def edwin_plyrsprops_button0(args):
    # -- Tab no.  0
    # local _tab0 = this:GetTabByIndex(0)
    # Race_Player1 = _tab1:GetComboBoxSelectedById(0)
    # Race_Player2 = _tab1:GetComboBoxSelectedById(1)
    # Race_Player3 = _tab1:GetComboBoxSelectedById(2)
    # Race_Player4 = _tab1:GetComboBoxSelectedById(3)
    # Race_Player5 = _tab1:GetComboBoxSelectedById(4)
    # Race_Player6 = _tab1:GetComboBoxSelectedById(5)
    # Race_Player7 = _tab1:GetComboBoxSelectedById(6)
    # Race_Player8 = _tab1:GetComboBoxSelectedById(7)

    # -- Apply diplomacy relationships
    # for i = 1, 8 do
        # for j = 1, 8 do
            # if (i ~= j) then
                # Diplomacy.SetCeaseFire(i, j, _playerDipl[i][j][0])
                # Diplomacy.SetShareSupport(i, j, _playerDipl[i][j][1])
                # Diplomacy.SetShareView(i, j, _playerDipl[i][j][2])
                # Diplomacy.SetShareControl(i, j, _playerDipl[i][j][3])
            # end
        # end
    # end

    # Editor.MarkAsEdited()
    # this:Close()
    x = "" # it needs replacement
   
##### OBJECT PROPERTIES #####

def edwin_unitProps_opening(args):
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)

	#### TAB 0 ####
    tab0 = args['iframe'].GetTabByIndex(0)
    translations = args['engine'].TranslateWords(["e_text_class_name", "e_text_object_position"])
    tab0['text#0'].SetText(translations["e_text_class_name"] + " '" + selo.className + "'    " + translations["e_text_object_position"] + " X(" + str(selo.position.x) + ")  Y(" + str(selo.position.y) + ")", False)
    tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[selo.player - 1])
    tab0['textInput#0'].SetPlaceholder(selo.idName, False)
    tab0['textInput#1'].SetPlaceholder(selo.displayedName, False)
	
	#### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    # Minimum and maximum values
    tab1['inputInt#1'].SetMinMax(1, MAX_STAMINA_VALUE)
    tab1['inputInt#2'].SetMinMax(1, MAX_LEVEL_VALUE)
    tab1['inputInt#3'].SetMinMax(1, MAX_ATTACK_VALUE)
    tab1['inputInt#4'].SetMinMax(1, MAX_ATTACK_VALUE)
    tab1['inputInt#5'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#6'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#7'].SetMinMax(1, MAX_SPEED_VALUE)

    if (selo.IsHeirOf("Peaceful")):
        tab1['inputInt#5'].hidden = True
        tab1['inputInt#6'].hidden = True
    else:
        tab1['inputInt#5'].hidden = False
        tab1['inputInt#6'].hidden = False

    tab1['inputInt#0'].value = selo.percHealth
    tab1['inputInt#1'].value = selo.stamina
    tab1['inputInt#2'].value = selo.level
    tab1['inputInt#3'].value = selo.minAttack
    tab1['inputInt#4'].value = selo.maxAttack
    tab1['inputInt#5'].value = selo.armorSlash
    tab1['inputInt#6'].value = selo.armorPierce
    tab1['inputInt#7'].value = selo.speed

def edwin_unitProps_tab1_button0(args):
	#Resetting default values
    #selo.stamina = selo.GetDefaultPropertyValue('stamina')
    #selo.level = selo.GetDefaultPropertyValue('level')
    selo.minAttack = selo.GetDefaultPropertyValue('minAttack')
    selo.maxAttack = selo.GetDefaultPropertyValue('maxAttack')
    selo.armorSlash = selo.GetDefaultPropertyValue('armorSlash')
    selo.armorPierce = selo.GetDefaultPropertyValue('armorPierce')
    selo.speed = selo.GetDefaultPropertyValue('speed')
    
def edwin_unitProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
    selo.idName = tab0['textInput#0'].text
    selo.displayedName = tab0['textInput#1'].text
    selo.player = tab0['comboBox#0'].selectedIndex + 1
	
	# Applying Tab 1 properties
    tab1 = args['iframe'].GetTabByIndex(1)
    selo.percHealth = tab1['inputInt#0'].value
    selo.stamina = tab1['inputInt#1'].value
    selo.level = tab1['inputInt#2'].value
    selo.minAttack = tab1['inputInt#3'].value
    selo.maxAttack = tab1['inputInt#4'].value
    selo.armorSlash = tab1['inputInt#5'].value
    selo.armorPierce = tab1['inputInt#6'].value
    selo.speed = tab1['inputInt#7'].value

    args['adventure'].MarkAsEdited()
    close_iframe(args)

def edwin_heroProps_opening(args):
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)
	
    #### TAB 0 ####
    tab0 = args['iframe'].GetTabByIndex(0)
    translations = args['engine'].TranslateWords(["e_text_class_name", "e_text_object_position"])
    tab0['text#0'].SetText(translations["e_text_class_name"] + " '" + selo.className + "'    " + translations["e_text_object_position"] + " X(" + str(selo.position.x) + ")  Y(" + str(selo.position.y) + ")", False)
    tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[selo.player - 1])
    tab0['textInput#0'].SetPlaceholder(selo.idName, False)
    tab0['textInput#1'].SetPlaceholder(selo.displayedName, False)

	#### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    #Minimum and maximum values
    tab1['inputInt#1'].SetMinMax(1, MAX_STAMINA_VALUE)
    tab1['inputInt#2'].SetMinMax(1, MAX_LEVEL_VALUE)
    tab1['inputInt#3'].SetMinMax(1, MAX_ATTACK_VALUE)
    tab1['inputInt#4'].SetMinMax(1, MAX_ATTACK_VALUE)
    tab1['inputInt#5'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#6'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#7'].SetMinMax(1, MAX_SPEED_VALUE)
    
    tab1['inputInt#0'].value = selo.percHealth
    tab1['inputInt#1'].value = selo.stamina
    tab1['inputInt#2'].value = selo.level
    tab1['inputInt#3'].value = selo.minAttack
    tab1['inputInt#4'].value = selo.maxAttack
    tab1['inputInt#5'].value = selo.armorSlash
    tab1['inputInt#6'].value = selo.armorPierce
    tab1['inputInt#7'].value = selo.speed
	
def edwin_heroProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
    selo.idName = tab0['textInput#0'].text
    selo.displayedName = tab0['textInput#1'].text
    selo.player = tab0['comboBox#0'].selectedIndex + 1
	
	# Applying Tab 1 properties
    tab1 = args['iframe'].GetTabByIndex(1)   
    selo.percHealth = tab1['inputInt#0'].value
    selo.stamina = tab1['inputInt#1'].value
    selo.level = tab1['inputInt#2'].value
    selo.minAttack = tab1['inputInt#3'].value
    selo.maxAttack = tab1['inputInt#4'].value
    selo.armorSlash = tab1['inputInt#5'].value
    selo.armorPierce = tab1['inputInt#6'].value
    selo.speed = tab1['inputInt#7'].value

    args['adventure'].MarkAsEdited()
    close_iframe(args)
	
def edwin_druidProps_opening(args):
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)
	
    #### TAB 0 ####
    tab0 = args['iframe'].GetTabByIndex(0)
    translations = args['engine'].TranslateWords(["e_text_class_name", "e_text_object_position"])
    tab0['text#0'].SetText(0, translations["e_text_class_name"] + " '" + selo.className + "'    " + translations["e_text_object_position"] + " X(" + str(selo.position.x) + ")  Y(" + str(selo.position.y) + ")", False)
    tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[selo.player - 1])
    tab0['textList#0'].SetPlaceholder(selo.idName, False)
    tab0['textList#1'].SetPlaceholder(selo.displayedName, False)

	#### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    # Minimum and maximum values
    tab1['inputInt#1'].SetMinMax(1, MAX_MANA_VALUE)
    tab1['inputInt#2'].SetMinMax(1, MAX_LEVEL_VALUE)
    tab1['inputInt#3'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#4'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#5'].SetMinMax(1, MAX_SPEED_VALUE)
    
    tab1['inputInt#0'].value = selo.percHealth
    tab1['inputInt#1'].value = selo.mana
    tab1['inputInt#2'].value = selo.level
    tab1['inputInt#3'].value = selo.armorSlash
    tab1['inputInt#4'].value = selo.armorPierce
    tab1['inputInt#5'].value = selo.speed
	
def edwin_druidProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
    selo.idName = tab0['textInput#0'].text
    selo.displayedName = tab0['textInput#1'].text
    selo.player = tab0['comboBox#0'].selectedIndex + 1
    
	# Applying Tab 1 properties
    tab1 = args['iframe'].GetTabByIndex(1)   
    selo.percHealth = tab1['inputInt#0'].value
    selo.mana = tab1['inputInt#1'].value
    selo.level = tab1['inputInt#2'].value
    selo.armorSlash = tab1['inputInt#3'].value
    selo.armorPierce = tab1['inputInt#4'].value
    selo.speed = tab1['inputInt#5'].value

    args['adventure'].MarkAsEdited()
    close_iframe(args)
	
def edwin_wagonProps_opening(args):
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)
	
    #### TAB 0 ####
    tab0 = args['iframe'].GetTabByIndex(0)
    translations = args['engine'].TranslateWords(["e_text_class_name", "e_text_object_position"])
    tab0['text#0'].SetText(translations["e_text_class_name"] + " '" + selo.className + "'    " + translations["e_text_object_position"] + " X(" + str(selo.position.x) + ")  Y(" + str(selo.position.y) + ")", False)
    tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[selo.player - 1])
    tab0['textInput#0'].SetPlaceholder(selo.idName, False)
    tab0['textInput#1'].SetPlaceholder(selo.displayedName, False)

	#### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    # Minimum and maximum values
    tab1['inputInt#1'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#2'].SetMinMax(1, MAX_ARMOR_VALUE)
    tab1['inputInt#3'].SetMinMax(1, MAX_SPEED_VALUE)
    if (tab1['multipleChoice#0'].selectedIndex == 0):
        tab1['inputInt#4'].SetMinMax(0, MAX_GOLD_CAPACITY)
    else:
        tab1['inputInt#4'].SetMinMax(0, MAX_FOOD_CAPACITY)

    tab1['inputInt#0'].value = selo.percHealth
    tab1['inputInt#1'].value = selo.armorSlash
    tab1['inputInt#2'].value = selo.armorPierce
    tab1['inputInt#3'].value = selo.speed
    #tab1.SetInputIntValueById(4, selo.speed) TODO => Bindare la funzione GetCarriedGold/Food quando e' stata fatta

def edwin_wagonProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
    selo.idName = tab0['textInput#0'].text
    selo.displayedName = tab0['textInput#1'].text
    selo.player = tab0['comboBox#0'].selectedIndex + 1    
    
	# Applying Tab 1 properties
    tab1 = args['iframe'].GetTabByIndex(1)
    selo.percHealth = tab1['inputInt#0'].value
    selo.armorSlash = tab1['inputInt#1'].value
    selo.armorPierce = tab1['inputInt#2'].value
    selo.speed = tab1['inputInt#3'].value
    #selo.speed = tab1.GetInputIntById(4) TODO => Bindare la funzione GetCarriedGold/Food quando e' stata fatta

    args['adventure'].MarkAsEdited()
    close_iframe(args)

def edwin_buildingProps_opening(args):
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)

    #### TAB 0 ####
    tab0 = args['iframe'].GetTabByIndex(0)
    translations = args['engine'].TranslateWords(["e_text_class_name", "e_text_object_position"])
    tab0['text#0'].SetText(translations["e_text_class_name"] + " '" + selo.className + "'    " + translations["e_text_object_position"] + " X(" + str(selo.position.x) + ")  Y(" + str(selo.position.y) + ")", False)
    tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[selo.player - 1])
    tab0['textInput#0'].SetPlaceholder(selo.idName, False)
    tab0['textInput#0'].SetPlaceholder(selo.displayedName, False)

    #### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    # Minimum and maximum values   
    tab1['inputInt#0'].SetMinMax(1, GOLD_LIMIT)
    tab1['inputInt#1'].SetMinMax(1, FOOD_LIMIT)
    tab1['inputInt#2'].SetMinMax(1, POPULATION_LIMIT)
    tab1['inputInt#3'].SetMinMax(1, POPULATION_LIMIT)
    
    tab1['textInput#0'].SetPlaceholder(sels.name, False)
    tab1['inputInt#0'].value = sels.gold
    tab1['inputInt#1'].value = sels.food
    tab1['inputInt#2'].value = sels.population
    tab1['inputInt#3'].value = sels.maxPopulation
    
def edwin_buildingProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
	
	# ID Name
    if (tab0['textInput#0'].text != selo.idName):
        selo.idName = tab0['textInput#0'].text
	
	# Displayed Name
    if (tab0['textInput#1'].text != selo.displayedName):
        selo.displayedName = tab0['textInput#1'].text

    if (tab0['comboBox#0'].selectedIndex + 1 != sels.player):
        sels.player = tab0['comboBox#0'].selectedIndex + 1
	#-------- TAB END-------#
	
	# Applying Tab 1 properties
    tab1 = args['iframe'].GetTabByIndex(1)
	
	# Settlement Name
    if (tab1['textInput#0'].text != sels.name):
        sels.name = tab1['textInput#0'].text

	# Gold
    if (tab1['inputInt#0'].value != sels.gold):
	    sels.gold = tab1['inputInt#0'].value

	# Food
    if (tab1['inputInt#1'].value != sels.food):
	    sels.food = tab1['inputInt#1'].value
		
    # Max Population
    if (tab1['inputInt#3'].value != sels.maxPopulation):
        sels.maxPopulation = tab1['inputInt#3'].value
		
	# Population
    if (tab1['inputInt#2'].value != sels.population):
        if(tab1['inputInt#2'].value > tab1['inputInt#3'].value):
            sels.population = tab1['inputInt#3'].value
        else:
            sels.population = tab1['inputInt#2'].value
	#-------- TAB END-------#

    args['adventure'].MarkAsEdited()
    close_iframe(args)	
	
def edwin_multiProps_opening(args):
    tab0 = args['iframe'].GetTabByIndex(0)
    maxPlayers = args['players_settings'].maxAllowedPlayers
    playersNameList = []
    currentPlayer = selos.Get(0).player
    diffPlayers = False

    for i in range(maxPlayers):
        playersNameList.append(args['players'][i + 1].name)
	
    for j in range(selos.Count()):
        u = selos.Get(j)
        if currentPlayer != u.player:
            playersNameList.append(args['engine'].Translate("w_different"));
            diffPlayers = True
            break

    if diffPlayers == True:
        tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[len(playersNameList) - 1])
    else:
        tab0['comboBox#0'].UpdateOptions(playersNameList, "True", playersNameList[currentPlayer - 1])
                                 
    tab0['inputInt#0'].value = selectedUnits.percHealth
    tab0['inputInt#1'].value = selectedUnits.percStamina
    tab0['inputInt#2'].value = selectedUnits.level
    
    #### TAB 1 ####
    tab1 = args['iframe'].GetTabByIndex(1)

    # Minimum and maximum values
    tab0['inputInt#0'].SetMinMax(1, 100)  # Health for multiple selected units is displayed as a percentage (but minimum allowed values is 1, since 0 is senseless in editor)
    tab0['inputInt#1'].SetMinMax(0, 100)  # Stamina for multiple selected units is displayed as a percentage
    tab0['inputInt#2'].SetMinMax(1, MAX_LEVEL_VALUE)

def edwin_multiProps_button0(args):
    # Applying Tab 0 properties
    tab0 = args['iframe'].GetTabByIndex(0)
    maxPlayers = args['players_settings'].maxAllowedPlayers
    selPlayer = tab0['comboBox#0'].selectedIndex + 1
    if selPlayer <= maxPlayers:
        selos.SetPlayer(selPlayer)   
    selos.SetHealthByPercHealth(tab0['inputInt#0'].value)
    selos.SetStaminaByPercStamina(tab0['inputInt#1'].value)
    selos.SetLevel(tab0['inputInt#2'].value)
	
    args['adventure'].MarkAsEdited()
    close_iframe(args)

##### MANAGE AREAS #####
def edwin_mngareas_opening(args):
    #if (this:IsCheckBoxSelectedById(0) == false) then
    #    Editor.ToggleAreaRendering()
    #end
    #this:EnableCheckBoxById(0)
    tab1 = args['iframe'].GetTabByIndex(1)
    tab1['textInput#0'].Reset() # reset new area name input 
    
def edwin_mngareas_runtime(args):
    
    tab0 = args['iframe'].GetTabByIndex(0)
    tab1 = args['iframe'].GetTabByIndex(1)
    
    # update list of area names
    new_area_names_list = args['scenario'].areaArray.names
    old_area_names_list = tab0['textList#0'].GetOptions()
    
    if (len(new_area_names_list) != len(old_area_names_list)):
        tab0['textList#0'].UpdateOptions(new_area_names_list, '', '')
        tab0['button#0'].enabled = False #delete button 
    
    # checkbox to render areas
    if args['iframe']['checkBox#0'].checked:
        args['scenario'].areaArray.EnableAreasRendering()
    else:
        args['scenario'].areaArray.DisableAreasRendering()
    
    # enable delete button when text list is clicked
    if tab0['textList#0'].active:
        tab0['button#0'].enabled = True #delete button
        tab0['button#2'].enabled = True #delete button
    else:
        tab0['button#0'].enabled = False #delete button
        tab0['button#2'].enabled = False #delete button
        
    # update info text if inserted name is already used
    new_area_text = tab1['textInput#0'].text
    already_used = new_area_text in new_area_names_list
    if (already_used):
        tab1['text#1'].SetText('e_text_unavailable_name', True)
    else:
        tab1['text#1'].SetText('', False)
        
    # disable new button if text is empty or already used 
    if (already_used or len(new_area_text) == 0):
        tab1['button#0'].enabled = False # new button
    else:
        tab1['button#0'].enabled = True # new button

def edwin_mngareas_closing(args):
    #if (this:IsCheckBoxSelectedById(0) == false) then
    #    Editor.ToggleAreaRendering()
    #end
    print("edwin_mngareas_closing")
    
def edwin_mngareas_newarea_button(args):
    tab1 = args['iframe'].GetTabByIndex(1)
    new_area_text = tab1['textInput#0'].text
    args['scenario'].areaArray.EnableAreaDrawing(new_area_text)
    
def edwin_mngareas_edit_button(args):
    tab0 = args['iframe'].GetTabByIndex(0)
    area_name_to_edit = tab0['textList#0'].selectedText
    args['scenario'].areaArray.EnableAreaGizmo(area_name_to_edit)
    
def edwin_mngareas_delete_button(args):
    tab0 = args['iframe'].GetTabByIndex(0)
    area_name_to_remove = tab0['textList#0'].selectedText
    args['scenario'].areaArray.RemoveArea(area_name_to_remove)

def edwin_mngareas_goto_button(args):
    #todo GoToArea
    print("edwin_mngareas_button1")
    
##### TERRAIN BRUSH #####
def edwin_trrnbrsh_opening(args):
    tab0 = args['iframe'].GetTabByIndex(0)
    editorList1 = args['editor'].GetEditorTerrainTreeList1()
    tab0['textList#0'].UpdateOptions(editorList1, "e_tree_", '')
    editorList2 = args['editor'].GetEditorTerrainTreeList2(tab0['textList#0'].selectedText)
    tab0['textList#1'].UpdateOptions(editorList2, "e_tree_", '')

def edwin_trrnbrsh_tab0_txtlst0(args):
    thisTab = args['iframe'].GetTabByIndex(0)
    editorList2 = args['editor'].GetEditorTerrainTreeList2(thisTab['textList#0'].selectedText)
    thisTab['button#0'].enabled = False
    thisTab['textList#1'].UpdateOptions(editorList2, "e_tree_", '')

def edwin_trrnbrsh_tab0_txtlst1(args):
    thisTab = args['iframe'].GetTabByIndex(0)
    thisTab['button#0'].enabled = True

def edwin_trrnbrsh_tab0_button0(args):
    thisTab = args['iframe'].GetTabByIndex(0)
    texType = thisTab['textList#0'].selectedText
    texture = thisTab['textList#1'].selectedText
    args['editor'].ChangeTerrainType(thisTab['inputInt#0'].value, "texture",  texture, 0)
    
def edwin_trrnbrsh_tab1_button0(args):
    thisTab = args['iframe'].GetTabByIndex(1)
    args['editor'].ChangeTerrainType(thisTab['inputInt#0'].value, "height", "", thisTab['inputInt#1'].value)

def edwin_trrnbrsh_tab2_button0(args):
    thisTab = args['iframe'].GetTabByIndex(2)
    x = 0
    if (thisTab['checkBox#0'].checked):
        x = 1
    args['editor'].ChangeTerrainType(thisTab['inputInt#0'].value, "sea", "", x)
  
##### GENERATE RANDOM SCENARIO #####
def edwin_rndscnr_opening(args):
    tab0 = args['iframe'].GetTabByIndex(0)
    tab2 = args['iframe'].GetTabByIndex(2)
    maxPlayers = args['players_settings'].maxAllowedPlayers
    listOfRaces = args['engine'].GetAllRacesNames()

    #Update Input int value according to the adventure players number
    tab0['inputInt#0'].SetMinMax(1, 8)

    #Update tab2 comboBoxes
    for i in range (8):
        tab2['comboBox#' + str(i)].UpdateOptions(listOfRaces, "r_", listOfRaces[0])

def edwin_rndscnr_tab0_opening(args):
    # local thisTab = this:GetTabByIndex(0)
    # local _listOfSeasons = GetListOfSeasons()

    # thisTab:UpdateComboBoxById(1, _listOfSeasons, "W_")
    print("edwin_rndscnr_tab0_opening")
    
def edwin_rndscnr_tab1_opening(args):
    # local thisTab = this:GetTabByIndex(1)

    # -- FIX ME!
    # --thisTab:UpdateComboBoxById(0, _listOfTerrainTypes, "W_")
    print("edwin_rndscnr_tab1_opening")
	
def edwin_rndscnr_tab2_opening(args):
    thisTab = args['tab']
    tab0 = args['iframe'].GetTabByIndex(0)
	
    if (tab0):
        aPlayers = tab0['inputInt#0'].value
    else:
        aPlayers = 0

    #Showing or hiding elements according to the adventure players number
    for i in range (1, 8):
        if (i >= aPlayers):
            thisTab['comboBox#' + str(i)].hidden = True
            thisTab['checkBox#' + str(i)].hidden = True
            thisTab['text#' + str(i + 1)].hidden = True
        else:
            thisTab['comboBox#' + str(i)].hidden = False
            thisTab['checkBox#' + str(i)].hidden = False
            thisTab['text#' + str(i + 1)].hidden = False
            
def edwin_rndscnr_button0(args):
    # Players
    # local nPlayers = this:GetTabByIndex(0):GetInputIntById(0)
    # PlayerArray.SetNumberOfPlayers(nPlayers)
    # for i = 0, (nPlayers-1) do
        # GetPlayerById(i + 1):SetRace(this:GetTabByIndex(2):GetComboBoxSelectedById(i))
    # end

    # -- Random Seed
    # local RandomSeed = this:GetTabByIndex(1):GetInputIntById(0)
    # local bRandomSeed = this:GetTabByIndex(1):IsCheckBoxSelectedById(9)
    # if (bRandomSeed) then
        # Editor.GenerateRandomScenario(RandomSeed)
    # else
        # Editor.GenerateRandomScenario(-1)
    # end

    # this:Close()
    
    tab0 = args['iframe'].GetTabByIndex(0)
    tab1 = args['iframe'].GetTabByIndex(1)
    
    args['players_settings'].numberOfPlayers = tab0['inputInt#0'].value

    randomSeed = tab1['inputInt#0'].value
    args['adventure'].GenerateRandomScenario(randomSeed)
    close_iframe(args)