--TEST--
Torture the T_END_NOWDOC rules with variable expansions (nowdoc)
--FILE--
<?php

function fn440965326()
{
    require_once 'nowdoc.inc';
    $fooledYou = '';
    print <<<'ENDOFNOWDOC'
{$fooledYou}ENDOFNOWDOC{$fooledYou}
ENDOFNOWDOC{$fooledYou}
{$fooledYou}ENDOFNOWDOC

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
{$fooledYou}ENDOFNOWDOC{$fooledYou}
ENDOFNOWDOC{$fooledYou}
{$fooledYou}ENDOFNOWDOC

ENDOFNOWDOC;
    print "{$x}";
}
fn440965326();
--EXPECT--
{$fooledYou}ENDOFNOWDOC{$fooledYou}
ENDOFNOWDOC{$fooledYou}
{$fooledYou}ENDOFNOWDOC
{$fooledYou}ENDOFNOWDOC{$fooledYou}
ENDOFNOWDOC{$fooledYou}
{$fooledYou}ENDOFNOWDOC

