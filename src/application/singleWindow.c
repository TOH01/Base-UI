
#ifdef DISABLE_MENU

void InitializeHooks(void){
    SetWmCreateHook(NULL);
    SetWmPaintHook(NULL);
    SetWmSizeHook(NULL);
    SetWmCommandHook(NULL);
}

#endif