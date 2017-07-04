--TEST--
Bug #43958 (class name added into the error message)
--FILE--
<?php

class MyClass
{
    public static function loadCode($p)
    {
        return include $p;
    }
}
function fn2026017149()
{
    MyClass::loadCode('file-which-does-not-exist-on-purpose.php');
}
fn2026017149();
--EXPECTF--
Warning: include(file-which-does-not-exist-on-purpose.php): failed to open stream: No such file or directory in %sbug43958.php on line %d

Warning: include(): Failed opening 'file-which-does-not-exist-on-purpose.php' for inclusion (include_path='%s') in %sbug43958.php on line %d

