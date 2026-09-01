import os
import dulwich.porcelain
import dulwich.repo

repo_dir = r"c:\Users\Janis Ole\Downloads\Projekt_Kirchner_Unterricht"

# 1. Git Repository initialisieren (falls noch nicht vorhanden)
if not os.path.exists(os.path.join(repo_dir, ".git")):
    print(f"Initialisiere neues Git-Repository in {repo_dir}...")
    repo = dulwich.repo.Repo.init(repo_dir)
else:
    print(f"Öffne bestehendes Git-Repository in {repo_dir}...")
    repo = dulwich.repo.Repo(repo_dir)

# 2. Temporäre Hilfsskripte löschen vor Commit
for temp_file in ["install_mingit.py", "generate_reqif.py"]:
    temp_path = os.path.join(repo_dir, temp_file)
    if os.path.exists(temp_path):
        os.remove(temp_path)

# 3. Alle Dateien zum Index hinzufügen
print("Füge alle Projektdateien zum Git-Index hinzu...")
dulwich.porcelain.add(repo_dir, ".")

# 4. Commit erstellen
author = b"Janis Ole <janis.ole@local>"
commit_msg = (
    b"feat: Initialer Projektstand - 2x Arduino Uno R4 Teststand\n\n"
    b"- System-Anforderungsspezifikation (Lastenheft VDI 2206 / VDI 2221)\n"
    b"- Hardware-Aufbauanleitung & Schaltplan fuer Arduino UNO R4\n"
    b"- Sender- & Empfaenger-Sketche (Moving Average, 70% Alarm, Watchdog)\n"
    b"- Modulare Parameter-Konfiguration (TeststandConfig.h)\n"
    b"- Interaktive Prozess-Simulation (Teststand_Sandbox_Simulation.html)\n"
    b"- Physical Architecture Board (Physische_Komponenten_Sandbox.html)\n"
    b"- OMG ReqIF-Modelldatei & Import-Anleitung fuer Eclipse Capella\n"
)

try:
    commit_id = dulwich.porcelain.commit(
        repo_dir,
        message=commit_msg,
        author=author,
        committer=author
    )
    print(f"Erfolgreicher Commit erstellt! Commit-ID: {commit_id.decode('utf-8') if isinstance(commit_id, bytes) else commit_id}")
except Exception as e:
    print(f"Commit-Status: {e}")

# 5. Status und Commit-Log ausgeben
print("\n--- GIT LOG ---")
for entry in dulwich.porcelain.log(repo_dir, max_entries=5):
    print(f"Commit: {entry.commit.id.decode('utf-8')}")
    print(f"Author: {entry.commit.author.decode('utf-8')}")
    print(f"Message: {entry.commit.message.decode('utf-8').strip()}")
