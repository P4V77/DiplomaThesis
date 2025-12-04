### První krůčky
Šablonu je možné přeložit ze souboru `main.tex` a obsahuje základní návod na použití systému LaTeX. Níže naleznete seznam úkolů, které byste měli někdy v průběhu tvorby všechny splnit.
- Jděte do šložky `styl` a upravte soubor `nastaveni.tex`.
- Pokud nemáte konzultanta, v souboru `styl/titulni.tex` smažte řádek 24.
- Nahraďte soubor `styl/zadani.pdf` vaším naskenovaným zadáním.
- Jakmile vytvoříte vlastní obsah, smažte přebytečné soubory (vše kromě `*.tex`) ve složkách `kapitola1` a `kapitola2`.

### Doporučení
Na práci vám doporučujeme online systém OverLeaf, který je plně kompatibilní s naší šablonou. Stačí se zaregistrovat, vytvořit projekt a nahrát tam jednotlivé soubory (pozor, stromová struktura projektu musí být totožná!). Následně je nutné v menu vybrat soubor, který se má volat jako hlavní, čímž je `main.tex`. Vyberete ho pomocí `Menu` -> `Main document`.

Pokud zvolíte vývoj offline, budete muset pravděpodobně ve `styl/styl.tex` odkomentovat řádek 111 (řádek, který definuje `backend=bibtex`) pro správné generování seznamu literatury.

### Stručné vysvětlení složek a souborů
- *kapitola1/* - vše, co se týká 1. kapitoly
- *kapitola2/* - vše, co se týká 2. kapitoly
- *prilohy/* - dokumenty do příloh (schémata, kódy, obrázky, data, ...)
- *styl/* - nastevní šablony a celkového vzhledu práce
   - `abstrakt_podekovani.tex` - zde vyplňte úvodní listy práce
   - `commands.tex` - pár nestandardních příkazů přidaných pro snazší práci s šablonou
   - `literatura.tex` - výpis literatury, zde není nutné nic měnit - literaturu olivníte pomocí souboru `bib.bib`
   - `math.tex` - pár nestandardních matematických definic, které můžete použít
   - `nastaveni.tex` - zde musíte vyplnit všechny položky !
   - `seznamy.tex` - definice, jak se mají vypisovat seznamy obrázků, tabulek, apod.
   - `styl.tex` - primární definice vzhledu a chování šablony
   - `titulni.tex` - definice titulní strany - nutné upravit pouze pokud nemáte konzultanta
   - `zadani.pdf` - tento soubor nahraďte skenem vašeho zadání
- *uvod/* - vše, co se týká úvodní kapitoly
- *zaver/* - vše, co se týká závěrečné kapitoly
- `.gitignore` - pokud byste šablonu rádi používali se systémem git, zde je definice souborů, které se mají být systémem ignorovány
- `bib.bib` - definice literatury - sem budete přidávat jednotlivé položky, které budete chtít citovat (detailně popsáno v samotné šabloně práce)
- `CHANGELOG.md` - popis změn této šablony
- `main.pdf` - do tohoto souboru se při každém překladu vytvoří PDF vaší práce
- `main.tex` - úvodní soubor na překlad celého dokumentu. Doporučujeme projít, zjistíte tím několik důležitých věcí o samotné struktuře a tvorbě dokumentu.
- `mcode.sty` - styl formátování kódu ze systému MATLAB
- `README.md` - nelze vyplnit - vedlo by na nekonečnou smyčku

### Zpětná vazba
Pokud jste v šabloně našli jakoukoliv chybu, kontaktujte prosím proděkana pro pedagogiku, který zprávu předá autorům.
