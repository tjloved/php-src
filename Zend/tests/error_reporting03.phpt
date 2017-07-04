--TEST--
testing @ and error_reporting - 3
--FILE--
<?php

function foo($arg)
{
    echo @$nonex_foo;
}
function bar()
{
    echo @$nonex_bar;
    throw new Exception("test");
}
function foo1()
{
    echo $undef1;
    error_reporting(E_ALL | E_STRICT);
    echo $undef2;
}
function fn611087852()
{
    error_reporting(E_ALL);
    try {
        @foo(@bar(@foo1()));
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn611087852();
--EXPECTF--	
Notice: Undefined variable: undef2 in %s on line %d
int(32767)
Done
