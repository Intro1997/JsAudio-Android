const micromatch = require("micromatch");

third_party_dir = ["node"];

module.exports = {
  "**/CMakeLists.txt": (file_path) => {
    const match = micromatch.not(file_path, third_party_dir);
    return [
      `cmake-format -c "./.vscode/cmake_format_config.json" -i ${match.join(" ")}`,
      "git add",
    ];
  },
  "*.{cpp, c, hpp, h, cc}": (file_path) => {
    const match = micromatch.not(file_path, third_party_dir);
    return [
      `clang-format -i ${match.join(" ")}`,
      `clang-tidy -p ./.vscode ${match.join(" ")}`,
      "git add",
    ];
  },
  "*.js": ["prettier --write"],
  "*.py": ["prettier --write"],
};
