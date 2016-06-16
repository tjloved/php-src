--TEST--
Bug #39018 [2] (Error control operator '@' fails to suppress "Uninitialized string offset")
--FILE--
<?php

function fn329312019()
{
    error_reporting(E_ALL);
    $foo = 'test';
    $x = @$foo[6];
    print @($foo[100] + $foo[130]);
    print "\nDone\n";
}
fn329312019();
--EXPECT--
0
Done
