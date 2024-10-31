All cases come from [wpt(web-platform-test)](https://github.com/web-platform-tests/wpt), which is descriped in [mozilla wiki](https://wiki.mozilla.org/Auto-tools/Projects/web-platform-tests). I think this a reliable repo to test Web Audio API.

Because we can't use broswer env in NodeJS env, and audio tests are usually independent of UI, so we make some modifications on wpt audio test to adapt this repo.

A rough description of the changes is listed below, and detailed descriptions can be found in the comments inside each test case.

# Modification

## OfflineAudioContext

We do not append following test, because these tests need broswer env that we won't support:

1. [offlineaudiocontext-detached-execution-context.html](https://github.com/web-platform-tests/wpt/blob/master/webaudio/the-audio-api/the-offlineaudiocontext-interface/offlineaudiocontext-detached-execution-context.html)
2. [startrendering-after-discard.html](https://github.com/web-platform-tests/wpt/blob/master/webaudio/the-audio-api/the-offlineaudiocontext-interface/startrendering-after-discard.html)

## AudioParam

We do not support any wpt test onf AudioParam, we only implementation data member of AudioParam.
