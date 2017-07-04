--TEST--
Bug #47516 (nowdoc can not be embed in heredoc but can be embed in double quote)
--FILE--
<?php

function fn2139183357()
{
    $s = 'substr';
    $htm = <<<DOC
{$s(<<<'ppp'
abcdefg
ppp
, 0, 3)}
DOC;
    echo "{$htm}\n";
    $htm = <<<DOC
{$s(<<<ppp
abcdefg
ppp
, 0, 3)}
DOC;
    echo "{$htm}\n";
    $htm = "{$s(<<<'ppp'
abcdefg
ppp
, 0, 3)}\n";
    echo "{$htm}\n";
}
fn2139183357();
--EXPECT--
abc
abc
abc
