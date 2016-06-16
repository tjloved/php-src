--TEST--
Testinh heredoc syntax
--FILE--
<?php

function fn393055664()
{
    $a = <<<A
\tA;
;
 A;
\\;
A;
    var_dump(strlen($a) == 12);
}
fn393055664();
--EXPECT--
bool(true)
