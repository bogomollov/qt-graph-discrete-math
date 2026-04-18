## Вклад в проект

1. Создать ветку для задачи
```bash
git checkout -b <type>/your-task
```

2. Сделать изменения и закоммитить
```bash
git commit -m "<type>(<scope>): <description>"
```

3. Запушить ветку
```bash
git push -u origin <type>/your-task
```

4. Создать Pull Request

Через CLI:
```bash
gh pr create --base main --head <type>/your-task
```

или через браузер:
- открыть репозиторий на GitHub
- нажать Compare & pull request

5. Дождаться успешного прохождения CI

6. Сделать merge Pull Request в браузере