--TEST--
Bug #64515 (Memoryleak when using the same variablename 2times in function declaration) (PHP7)
--FILE--
<?php

function foo($unused = null, $unused = null, $arg = array())
{
    return 1;
}
function fn775584437()
{
    foo();
    echo "okey";
}
fn775584437();
--EXPECTF--
Fatal error: Redefinition of parameter $unused in %sbug64515.php on line %d
