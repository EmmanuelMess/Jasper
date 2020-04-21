#include "lexer.hpp"

#include <iostream>
#include <string>

#include <cstdlib>

bool is_identifier_start_char (char c) {
	return isalpha(c) || c == '_';
}

bool is_identifier_char (char c) {
	return isalpha(c) || c == '_' || isdigit(c);
}

char Lexer::char_at(int index) {
	return index < int(m_source.size()) ? m_source[index] : '\0';
}

void Lexer::push_token(token_type tt, int width) {
	Token t = { tt, {}, m_current_line, m_current_column, m_current_line,
		m_current_column + width };

	m_tokens.push_back(t);

	m_source_index += width;
	m_current_column += width;
}

void Lexer::consume_token() {

	switch (current_char()) {

	case '+':
		switch (next_char()) {
		case '+':
			push_token(token_type::INCREMENT, 2);
			break;
		case '=':
			push_token(token_type::ADD_TO, 2);
			break;
		default:
			push_token(token_type::ADD, 1);
		}
		break;

	case '-':
		switch (next_char()) {
		case '-':
			push_token(token_type::DECREMENT, 2);
			break;
		case '=':
			push_token(token_type::SUB_TO, 2);
			break;
		default:
			push_token(token_type::SUB, 1);
		}
		break;

	case '*':
		switch (next_char()) {
		case '=':
			push_token(token_type::MUL_TO, 2);
			break;
		default:
			push_token(token_type::MUL, 1);
		}
		break;

	case '/':
		switch (next_char()) {
		case '=':
			push_token(token_type::DIV_TO, 2);
			break;
		default:
			push_token(token_type::DIV, 1);
		}
		break;

	case '&':
		switch (next_char()) {
		case '&':
			push_token(token_type::LOGIC_AND, 2);
			break;
		case '=':
			push_token(token_type::AND_TO, 2);
			break;
		default:
			push_token(token_type::AND, 1);
		}
		break;

	case '|':
		switch (next_char()) {
		case '|':
			push_token(token_type::LOGIC_IOR, 2);
			break;
		case '=':
			push_token(token_type::IOR_TO, 2);
			break;
		default:
			push_token(token_type::IOR, 1);
		}
		break;

	case '^':
		switch (next_char()) {
		/* case '^': push_token(token_type::LOGIC_XOR, 2); break; */
		case '=':
			push_token(token_type::XOR_TO, 2);
			break;
		default:
			push_token(token_type::XOR, 1);
		}
		break;

	case ':':
		switch (next_char()) {
		case '=':
			push_token(token_type::DECLARE_ASSIGN, 2);
			break;
		default:
			push_token(token_type::DECLARE, 1);
		}
		break;

	case '=':
		switch (next_char()) {
		case '=':
			push_token(token_type::EQUAL, 2);
			break;
		default:
			push_token(token_type::ASSIGN, 1);
		}
		break;

	case '<':
		switch (next_char()) {
		case '=':
			push_token(token_type::LTE, 2);
			break;
		default:
			push_token(token_type::LT, 1);
		}
		break;

	case '>':
		switch (next_char()) {
		case '=':
			push_token(token_type::GTE, 2);
			break;
		default:
			push_token(token_type::GT, 1);
		}
		break;

	case '!':
		switch (next_char()) {
		case '=':
			push_token(token_type::NOT_EQUAL, 2);
			break;
		default:
			push_token(token_type::LOGIC_COMPL, 1);
		}
		break;

	case '(':
		push_token(token_type::PAREN_OPEN, 1);
		break;
	case ')':
		push_token(token_type::PAREN_CLOSE, 1);
		break;
	case '{':
		push_token(token_type::BRACE_OPEN, 1);
		break;
	case '}':
		push_token(token_type::BRACE_CLOSE, 1);
		break;
	case '[':
		push_token(token_type::BRACKET_OPEN, 1);
		break;
	case ']':
		push_token(token_type::BRACKET_CLOSE, 1);
		break;
	case '~':
		push_token(token_type::COMPL, 1);
		break;
	case ';':
		push_token(token_type::SEMICOLON, 1);
		break;
	case '.':
		push_token(token_type::DOT, 1);
		break;
	case ',':
		push_token(token_type::COMMA, 1);
		break;
	case '"':
		{
			m_source_index += 1;
			m_current_column += 1;

			int i0 = m_source_index;
			int c0 = m_current_column;
			int l0 = m_current_line;

			// TODO: support escape sequences
			int len = 0;
			while ((not done()) && current_char() != '"') {
				len += 1;
				m_source_index += 1;
				if(current_char() == '\n'){
					m_current_line += 1;
					m_current_column = 0;
				}else{
					m_current_column += 1;
				}
			}

			if(current_char() != '"'){
				// TODO: report unmatched quote
				assert(0);
			}

			std::string text;
			assert(len == (m_source_index - i0));
			text.reserve(len);
			for(int i = i0; i != m_source_index; ++i){
				text.push_back(m_source[i]);
			}

			m_tokens.push_back({
				token_type::STRING,
				text,
				l0,
				c0,
				m_current_line,
				m_current_column });

			m_current_column += 1;
			m_source_index += 1;
		}
		break;

	default:

		// tokenize keywords
		// TODO: make table based?
		if (peek_char(0) == 'f') {
			if (peek_char(1) == 'n') {
				if (not is_identifier_char(peek_char(2))){
					push_token(token_type::KEYWORD_FN, 2);
					return;
				}
			} else if (peek_char(1) == 'o') {
				if (peek_char(2) == 'r') {
					if (not is_identifier_char(peek_char(3))){
						// FIXME: KEYWORD_FOR
						push_token(token_type::KEYWORD, 3);
						return;
					}
				}
			}
		}else if(peek_char(0) == 'i'){
			if (peek_char(1) == 'f') {
				if (not is_identifier_char(peek_char(2))){
					// FIXME: KEYWORD_IF
					push_token(token_type::KEYWORD, 2);
					return;
				}
			}
		}else if(peek_char(0) == 'r'){
			if(peek_char(1) == 'e'){
				if(peek_char(2) == 't'){
					if(peek_char(3) == 'u'){
						if(peek_char(4) == 'r'){
							if(peek_char(5) == 'n'){
								if (not is_identifier_char(peek_char(6))){
									// FIXME: KEYWORD_RETURN
									push_token(token_type::KEYWORD, 6);
									return;
								}
							}
						}
					}
				}
			}
		}

		if (is_identifier_start_char(current_char())) {
			std::string text;
			text.push_back(current_char());

			while (is_identifier_char(next_char())) {
				m_source_index += 1;
				text.push_back(current_char());
			}

			m_source_index += 1;
			m_current_column += text.size();

			m_tokens.push_back({
				token_type::IDENTIFIER,
				text,
				m_current_line,
				m_current_column - int(text.size()),
				m_current_line,
				m_current_column });

		} else if (isdigit(current_char())) {
			std::string text;
			text.push_back(current_char());

			while (isdigit(next_char())) {
				m_source_index += 1;
				m_current_column += 1;
				text.push_back(current_char());
			}

			if (next_char() == '.') {
				m_source_index += 1;
				m_current_column += 1;
				text.push_back(current_char());
			}

			while (isdigit(next_char())) {
				m_source_index += 1;
				m_current_column += 1;
				text.push_back(current_char());
			}

			m_source_index += 1;
			m_current_column += 1;
			m_tokens.push_back({
				token_type::NUMBER,
				text,
				m_current_line,
				m_current_column - int(text.size()),
				m_current_line,
				m_current_column });

		} else {
			if (current_char() == '\n') {
				m_current_line += 1;
				m_current_column = 0;
			} else {
				m_current_column += 1;
			}
			m_source_index += 1;
		}
	}
}

void Lexer::advance() {
	m_token_index += 1;
	while (m_token_index >= int(m_tokens.size())) {
		if (done())
			break;
		consume_token();
	}
}

void Lexer::regress() {
	assert(m_token_index > 0);
	m_token_index -= 1;
}

Token const& eof () {
	static Token t = {
		token_type::END,
		"(EOF)",
		-1,-1,
		-1,-1
	};
	return t;
}

Token const& Lexer::token_at(int index) {
	while (!done() && index >= int(m_tokens.size())) {
		consume_token();
	}

	if (done() && index >= int(m_tokens.size())) {
		return eof();
	}

	return m_tokens.at(index);
}

