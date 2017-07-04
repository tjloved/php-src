--TEST--
Bug #22592 (Cascading assignments to strings with curly braces broken)
--FILE--
<?php

function fn1332123880()
{
    $wrong = $correct = 'abcdef';
    $t = $x[] = 'x';
    var_dump($correct);
    var_dump($wrong);
    $correct[1] = '*';
    $correct[3] = '*';
    $correct[5] = '*';
    // This produces the
    $wrong[1] = $wrong[3] = $wrong[5] = '*';
    var_dump($correct);
    var_dump($wrong);
}
fn1332123880();
--EXPECT--
string(6) "abcdef"
string(6) "abcdef"
string(6) "a*c*e*"
string(6) "a*c*e*"
