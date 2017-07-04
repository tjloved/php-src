--TEST--
Test variable occuring on both LHS and RHS of list()
--FILE--
<?php

function fn793728945()
{
    $a = [1, 2, 3];
    list($a, $b, $c) = $a;
    var_dump($a, $b, $c);
    $b = [1, 2, 3];
    list($a, $b, $c) = $b;
    var_dump($a, $b, $c);
    $c = [1, 2, 3];
    list($a, $b, $c) = $c;
    var_dump($a, $b, $c);
    $a = [[1, 2], 3];
    list(list($a, $b), $c) = $a;
    var_dump($a, $b, $c);
    $b = [[1, 2], 3];
    list(list($a, $b), $c) = $b;
    var_dump($a, $b, $c);
    $b = [1, [2, 3]];
    list($a, list($b, $c)) = $b;
    var_dump($a, $b, $c);
    $c = [1, [2, 3]];
    list($a, list($b, $c)) = $c;
    var_dump($a, $b, $c);
}
fn793728945();
--EXPECT--
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
