--TEST--
testing @ and error_reporting - 4
--FILE--
<?php

function foo()
{
    echo $undef;
    error_reporting(E_ALL | E_STRICT);
}
function fn913236283()
{
    error_reporting(E_ALL);
    foo(@$var);
    var_dump(error_reporting());
    echo "Done\n";
}
fn913236283();
--EXPECTF--
Notice: Undefined variable: undef in %s on line %d
int(32767)
Done
