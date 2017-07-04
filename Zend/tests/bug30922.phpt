--TEST--
Bug #30922 (reflective functions crash PHP when interfaces extend themselves)
--FILE--
<?php

interface RecurisiveFooFar extends RecurisiveFooFar
{
}
class A implements RecurisiveFooFar
{
}
function fn1508353250()
{
    $a = new A();
    var_dump($a instanceof A);
    echo "ok\n";
}
fn1508353250();
--EXPECTF--
Fatal error: Interface RecurisiveFooFar cannot implement itself in %sbug30922.php on line %d
