def tech_research_condition(args):
    #if (cmd ~= nil) then
    #    if (this:GetSettlement():GetGold() &#60; cmd:GetGoldCost()) then
    #        cmd:SetRollover("UI_NotEnoughGold");
    #        return false;
    #    elseif (this:GetSettlement():GetFood() &#60; cmd:GetFoodCost()) then
    #        cmd:SetRollover("UI_NotEnoughFood");
    #        return false;
    #    elseif (this:GetSettlement():GetPopulation() &#60; cmd:GetPopulationCost() + 1) then
    #        cmd:SetRollover("UI_NotEnoughPopulation");
    #        return false;
    #    else
    #        cmd:ResetRollover();
    #        return true;
    #    end
    #else
    #    return false;
    #end
    return True
    
def tech_research_body(args):
    print('tech_research_body')
    