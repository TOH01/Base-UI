
#ifdef DISABLE_MENU

void InitializeHooks(){
    SetWmCreateHook(NULL);
    SetWmPaintHook(NULL);
    SetWmSizeHook(NULL);
    SetWmCommandHook(NULL);
}

#endif