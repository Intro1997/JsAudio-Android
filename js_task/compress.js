const fs = require("fs");
const uglify_js = require("uglify-js");

const preload_files = [
  {
    title: "NODE_LOGGER_PRELOAD_SCRIPT",
    path: "app/node_env/src/main/cpp/node_logger/preload_script.js",
  },
  {
    title: "NODE_INSTANCE_PRELOAD_SCRIPT",
    path: "app/node_env/src/main/cpp/node_env/preload_script.js",
  },
  {
    title: "NODE_HTTP_REQUIRE_SCRIPT",
    path: "app/node_env/src/main/cpp/node_http_require/preload_script.js",
  },
  {
    title: "AUDIO_PRELOAD_SCRIPT",
    path: "app/audio/src/main/cpp/audio/preload_script.js",
  },
];

const output_files = GetOutputFiles(preload_files);

CompressPreloadFiles(preload_files, output_files);

function GetOutputFiles(preload_files) {
  const output_files = [];
  preload_files.forEach((file) => {
    const path_str = file.path;
    const last_point_idx = path_str.lastIndexOf(".");
    output_files.push(path_str.slice(0, last_point_idx) + ".hpp");
  });
  return output_files;
}

function CompressPreloadFiles(preload_files, output_files) {
  let min_len = Math.min(preload_files.length, output_files.length);

  for (let i = 0; i < min_len; i++) {
    console.log(`Compressing ${preload_files[i].path}`);
    let file_content = fs.readFileSync(preload_files[i].path, "utf-8");
    let compressed_file_content = uglify_js.minify(file_content).code;
    compressed_file_content = compressed_file_content.replace(/"/g, `'`);
    const hpp_file_content = `#define ${preload_files[i].title} "${compressed_file_content}"`;
    fs.writeFileSync(
      output_files[i],
      hpp_file_content,
      { flag: "w" },
      (err) => {
        console.log(err);
      }
    );
    console.log("Compress finish.");
  }
}
