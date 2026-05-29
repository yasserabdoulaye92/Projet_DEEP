/* ==================================================================
   Podometer_RefreshDisplay : zones dynamiques uniquement
   ================================================================== */
void Podometer_RefreshDisplay(void)
{
    char buf[10];

    /* --- Compteur central --- */
    /* PRIu32 garantit le bon format pour uint32_t sur ARM GCC */
    snprintf(buf, sizeof(buf), "%5" PRIu32, sc.stepCount);

    ILI9341_FillRect(82, 102, 156, 46, COLOR_BG);
    ILI9341_WriteString(90, 112,
                        buf,
                        Font_16x26,
                        COLOR_SUCCESS, COLOR_BG);

    /* --- Barre de progression (100 pas = largeur totale) --- */
    uint16_t filled = (uint16_t)((sc.stepCount % 100) * (SCREEN_WIDTH - 22) / 100);

    /* Effacement de la barre */
    ILI9341_FillRect(11, 196, SCREEN_WIDTH - 22, 18, COLOR_BG);

    /* Remplissage proportionnel */
    if (filled > 0)
        ILI9341_FillRect(11, 196, filled, 18, COLOR_BAR);

    /* --- Série en cours --- */
    char serie[20];
    snprintf(serie, sizeof(serie), "%" PRIu32 " / 100", sc.stepCount % 100);

    ILI9341_FillRect(200, 170, 110, 20, COLOR_BG);
    ILI9341_WriteString(200, 170,
                        serie,
                        Font_11x18,
                        COLOR_ACCENT, COLOR_BG);
}

/* ==================================================================
   Podometer_Update : à appeler dans while(1)
   ================================================================== */
void Podometer_Update(void)
{
    uint32_t now = HAL_GetTick();

    /* Lecture MPU à ~50 Hz */
    if ((now - lastReadTick) >= PEDO_READ_PERIOD)
    {
        lastReadTick = now;
        MPU6050_Read(&mpu);
        StepCounter_Update_Internal(mpu.Ax, mpu.Ay, mpu.Az);
    }

    /* Rafraîchissement affichage */
    if ((now - lastDisplayTick) >= PEDO_DISPLAY_PERIOD
        || sc.stepCount != lastStepShown)
    {
        lastDisplayTick = now;
        lastStepShown   = sc.stepCount;
        Podometer_RefreshDisplay();
    }
}

/* ==================================================================
   Getter
   ================================================================== */
uint32_t Podometer_GetStepCount(void)
{
    return sc.stepCount;
}
