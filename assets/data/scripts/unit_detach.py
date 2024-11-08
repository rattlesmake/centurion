def unit_detach_condition(args):
    this = args['this']
    hero = this.GetHero()
    if hero is None:
        return False
    return True

def unit_detach_body(args):
    this = args['this']
    this.Detach()
