--TEST--
Bug #54585 (track_errors causes segfault)
--INI--
track_errors=On
--FILE--
<?php

function testing($source)
{
    unset($source[$cos]);
}
function fn1682875233()
{
    testing($_GET);
    echo "ok\n";
}
fn1682875233();
--EXPECTF--
Deprecated: Directive 'track_errors' is deprecated in Unknown on line %d

Notice: Undefined variable: cos in %sbug54585.php on line %d
ok
