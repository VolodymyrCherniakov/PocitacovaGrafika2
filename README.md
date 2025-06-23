# Semestrální práce z předmětu "Počítačová grafika 2"

## Nastavení projektu
1. Nainstalujte a integrujte [vcpkg](https://github.com/microsoft/vcpkg) do vašeho prostředí Visual Studio.
2. Pomocí vcpkg nainstalujte knihovnu **OpenCV 4.6** a přidejte ji podle prezentace "project setup".
3. V konfiguraci projektu přidejte cestu k ImGui v položce **Additional Include Directories**.
4. Otevřete soubor `my_app.sln` ve Visual Studiu (projekt již obsahuje všechna potřebná nastavení) a spusťte sestavení.

Nastavení grafiky (vsync, antialiasing a rozměry okna) se upravuje v souboru `config.json`.

## Ovládání
- **W, A, S, D** – pohyb kamery.
- **Mezerník** – skok.
- **Levý Shift** – rychlejší pohyb.
- **F10** – přepnutí VSyncu.
- **F11** – celoobrazovkový režim (uložení a obnovení pozice a velikosti okna).
- **H** – zobrazit/skrýt informační okno ImGui.
- **Pravé tlačítko myši** – uvolnit kurzor.
- **Kolečko myši** – změna FOV.
- **Prostřední tlačítko myši** – reset FOV na výchozí hodnotu.
- **Pohyb myši** – změna směru pohledu.
- **Esc** – ukončení aplikace.

## Splněné úkoly
Na základě dokumentu `Final project + eval.md` jsou splněny všechny požadavky kategorie **Essential**

Z volitelných úkolů je implementován systém částic (`ParticleSystem`).
