All cases come from [wpt(web-platform-test)](https://github.com/web-platform-tests/wpt), which is descriped in [mozilla wiki](https://wiki.mozilla.org/Auto-tools/Projects/web-platform-tests). I think this a reliable repo to test Web Audio API.

Because we can't use broswer env in NodeJS env, and audio tests are usually independent of UI, so we do some on wpt audio test to adapt JSAudio. All modifications are listed in each test file.
