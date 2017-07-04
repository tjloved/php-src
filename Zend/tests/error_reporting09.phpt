--TEST--
testing @ and error_reporting - 9
--FILE--
<?php

function bar()
{
    echo @$blah;
    echo $undef2;
}
function foo()
{
    echo @$undef;
    error_reporting(E_ALL | E_STRICT);
    echo $blah;
    return bar();
}
function fn1509720920()
{
    error_reporting(E_ALL);
    @foo();
    var_dump(error_reporting());
    echo "Done\n";
}
fn1509720920();
--EXPECTF--	
Notice: Undefined variable: blah in %s on line %d

Notice: Undefined variable: undef2 in %s on line %d
int(32767)
Done
