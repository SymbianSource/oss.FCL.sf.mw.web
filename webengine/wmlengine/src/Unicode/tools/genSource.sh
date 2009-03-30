#!/bin/bash

declare -a names
names=( LetterUppercase LetterLowercase LetterTitlecase LetterModifier \
	LetterOther MarkNonSpacing MarkSpacingCombining MarkEnclosing \
	NumberDecimalDigit NumberLetter NumberOther PunctuationConnector \
	PunctuationDash PunctuationOpen PunctuationClose PunctuationInitial \
	PunctuationFinal PunctuationOther SymbolMath SymbolCurrency \
	SymbolModifier SymbolOther SeparatorSpace SeparatorLine \
	SeparatorParagraph OtherControl OtherFormat OtherSurrogate \
	OtherPrivateUse OtherNotAssigned )
declare -a codes
codes=( Lu Ll Lt Lm Lo Mn Mc Me Nd Nl No Pc Pd Ps Pe Pi Pf Po Sm Sc Sk So Zs \
	Zl Zp Cc Cf Cs Co Cn )

cp Validate.h.pre ../test/Validate.h
cp Validate.c.pre ../test/Validate.c

# code generation
for (( index=0; index < ${#codes[*]}; index=index + 1 )); do
    echo ${names[$index]}

    space="`echo ${names[$index]}|sed -e 's/^.\{7,7\}//' -e 's/./ /g'`"

    sed >../src/NW_Unicode_${names[$index]}.c <NW_Unicode_%NAME%.c.pre \
		"s/%NAME%/${names[$index]}/g" 
    sed <UnicodeData.txt 's/^\([^;]*\);[^;]*;\([^;]*\);.*$/\2;\1/' | \
		sort | egrep "^${codes[$index]};" | sed 's/.*;//' | tee codes.${codes[$index]}.$$.bak | \
		./genRanges.exe | tee ranges.${codes[$index]}.$$.bak | sort >>../src/NW_Unicode_${names[$index]}.c
    sed >>../src/NW_Unicode_${names[$index]}.c <NW_Unicode_%NAME%.c.post \
		-e "s/%NAME%/${names[$index]}/g" -e "s/%SPACE%/${space}/g"

	numLines=`wc -l ranges.${codes[$index]}.$$.bak|sed 's/^ *\([0-9]*\).*$/\1/'`
	if [ "${numLines}" == 0 ]; then
		sed >../src/NW_Unicode_${names[$index]}.c <NW_Unicode_%EMPTY%.c \
			-e "s/%NAME%/${names[$index]}/g" -e "s/%SPACE%/${space}/g"
	fi

    sed >../include/NW_Unicode_${names[$index]}.h <NW_Unicode_%NAME%.h \
		"s/%NAME%/${names[$index]}/g" 
    sed >../include/NW_Unicode_${names[$index]}I.h <NW_Unicode_%NAME%I.h \
		"s/%NAME%/${names[$index]}/g"

	sed >../test/Validate${names[$index]}.c <Validate%NAME%.c.pre "s/%NAME%/${names[$index]}/g" 
	(for code in `cat codes.${codes[$index]}.$$.bak`; do echo -e "  if (!ValidateCode (0x${code}, &NW_Unicode_${names[$index]}_Class)) {\n    return NW_FALSE;\n  }"; done) >>../test/Validate${names[$index]}.c 
	sed >>../test/Validate${names[$index]}.c <Validate%NAME%.c.post "s/%NAME%/${names[$index]}/g" 

	echo >>../test/Validate.h "#include \"NW_Unicode_${names[$index]}.h\""
	echo >>../test/Validate.h "extern NW_Bool Validate${names[$index]} (void);"
	echo >>../test/Validate.h 

	sleep 1
	echo -e >>../test/Validate.c "  if (!Validate${names[$index]} ()) {\n    return NW_FALSE;\n  }"

	rm ranges.${codes[$index]}.$$.bak codes.${codes[$index]}.$$.bak
done

cat Validate.h.post >>../test/Validate.h
cat Validate.c.post >>../test/Validate.c
