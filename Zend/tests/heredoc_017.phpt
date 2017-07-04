--TEST--
Testinh heredoc syntax
--FILE--
<?php

function fn1779150249()
{
    $a = <<<A
\tA;
;
 A;
\\;
A;
    var_dump(strlen($a) == 12);
}
fn1779150249();
--EXPECT--
bool(true)
