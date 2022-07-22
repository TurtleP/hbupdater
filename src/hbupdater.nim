import cligen
include cligen/mergeCfgEnv

import strings

import ctrbin
import hacbin

when isMainModule:
    clCfg.version = strings.NimblePkgVersion

    try:
        cligen.dispatchMulti([ctr, help = CtrParamHelp], [hac, help = HacParamHelp])
    except Exception as e:
        echo(e.msg)
