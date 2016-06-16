--TEST--
Torture the T_END_HEREDOC rules with variable expansions (heredoc)
--FILE--
<?php

function fn1120235900()
{
    require_once 'nowdoc.inc';
    $fooledYou = '';
    print <<<ENDOFHEREDOC
{$fooledYou}ENDOFHEREDOC{$fooledYou}
ENDOFHEREDOC{$fooledYou}
{$fooledYou}ENDOFHEREDOC

ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
{$fooledYou}ENDOFHEREDOC{$fooledYou}
ENDOFHEREDOC{$fooledYou}
{$fooledYou}ENDOFHEREDOC

ENDOFHEREDOC;
    print "{$x}";
}
fn1120235900();
--EXPECT--
ENDOFHEREDOC
ENDOFHEREDOC
ENDOFHEREDOC
ENDOFHEREDOC
ENDOFHEREDOC
ENDOFHEREDOC
