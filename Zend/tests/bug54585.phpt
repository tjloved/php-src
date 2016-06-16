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
function fn83774984()
{
    testing($_GET);
    echo "ok\n";
}
fn83774984();
--EXPECTF--
Notice: Undefined variable: cos in %sbug54585.php on line %d
ok
