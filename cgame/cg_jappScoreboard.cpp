#include <functional>

#include "cg_local.h"
#include "ui/ui_public.h"
#include "ui/ui_shared.h"
#include "qcommon/qfiles.h"	// for STYLE_BLINK etc
#include "cg_media.h"
#include "ui/ui_fonts.h"

static int JP_GetScoreboardFont( void ) {
	return Q_clampi( FONT_SMALL, cg_newScoreboardFont.integer, FONT_NUM_FONTS );
}

static void DrawServerInfo( float fade ) {
	const Font fontLarge( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const Font fontSmall( FONT_JAPPMONO, cg_newScoreboardFontSize.value, false );
	const char *tmp = NULL;
	float y = 16.0f;
	float textWidth = 0.0f;
	float textHeight = 0.0f;
	vector4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	colour.a = fade;

	// map name
	tmp = va( "%s (%s)", CG_ConfigString( CS_MESSAGE ), cgs.mapnameClean );
	textHeight = fontSmall.Height( tmp );
	textWidth = fontSmall.Width( tmp );
	fontSmall.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
	y += textHeight;

	// server name
	tmp = cgs.japp.serverName;
	textHeight = fontSmall.Height( tmp );
	textWidth = fontSmall.Width( tmp );
	fontSmall.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
	y += textHeight;

	// gametype
	tmp = BG_GetGametypeString( cgs.gametype );
	textHeight = fontSmall.Height( tmp );
	textWidth = fontSmall.Width( tmp );
	fontSmall.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
	y += textHeight;

	switch ( cgs.gametype ) {
	case GT_FFA:
		if ( cgs.timelimit && cgs.fraglimit ) {
			tmp = va(
				"Until " S_COLOR_YELLOW "%i " S_COLOR_WHITE "frags or " S_COLOR_YELLOW "%i" S_COLOR_WHITE "/"
					S_COLOR_YELLOW "%i minutes",
				cgs.fraglimit, (cg.time - cgs.levelStartTime) / 60000, cgs.timelimit
			);
		}
		else if ( cgs.timelimit ) {
			tmp = va(
				"Until " S_COLOR_YELLOW "%i" S_COLOR_WHITE "/" S_COLOR_YELLOW "%i " S_COLOR_WHITE "minutes",
				(cg.time - cgs.levelStartTime) / 60000, cgs.timelimit
			);
		}
		else if ( cgs.fraglimit ) {
			tmp = va( "Until " S_COLOR_YELLOW "%i " S_COLOR_WHITE "frags", cgs.fraglimit );
		}
		else {
			tmp = "Playing forever!";
		}

		textHeight = fontLarge.Height( tmp );
		textWidth = fontLarge.Width( tmp );
		fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
		y += textHeight;
		break;

	case GT_CTF:
	case GT_CTY:
		if ( cgs.timelimit && cgs.capturelimit ) {
			tmp = va(
				"Until " S_COLOR_YELLOW "%i" S_COLOR_WHITE "/" S_COLOR_YELLOW "%i " S_COLOR_WHITE "captures or "
					S_COLOR_YELLOW "%i" S_COLOR_WHITE "/" S_COLOR_YELLOW "%i minutes",
				std::max( cgs.scores1, cgs.scores2 ), cgs.capturelimit, (cg.time - cgs.levelStartTime) / 60000,
				cgs.timelimit
			);
		}
		else if ( cgs.timelimit ) {
			tmp = va(
				"Until " S_COLOR_YELLOW "%i" S_COLOR_WHITE "/" S_COLOR_YELLOW "%i " S_COLOR_WHITE "minutes",
				(cg.time - cgs.levelStartTime) / 60000, cgs.timelimit
			);
		}
		else if ( cgs.capturelimit ) {
			tmp = va(
				"Until " S_COLOR_YELLOW "%i" S_COLOR_WHITE "/" S_COLOR_YELLOW "%i " S_COLOR_WHITE "captures",
				std::max( cgs.scores1, cgs.scores2 ), cgs.capturelimit
			);
		}
		else {
			tmp = "Playing forever!";
		}

		textHeight = fontLarge.Height( tmp );
		textWidth = fontLarge.Width( tmp );
		fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
		y += textHeight;
		//FALL THROUGH TO GENERIC TEAM GAME INFO!

	case GT_TEAM:
		if ( cgs.scores1 == cgs.scores2 ) {
			tmp = S_COLOR_YELLOW "Teams are tied";
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;

			tmp = va( S_COLOR_RED "%i " S_COLOR_WHITE "/ " S_COLOR_CYAN "%i", cgs.scores1, cgs.scores2 );
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;
		}
		else if ( cgs.scores1 > cgs.scores2 ) {
			tmp = S_COLOR_RED "Red " S_COLOR_WHITE "leads";
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;

			tmp = va( S_COLOR_RED "%i " S_COLOR_WHITE "/ " S_COLOR_CYAN "%i", cgs.scores1, cgs.scores2 );
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;
		}
		else {
			tmp = S_COLOR_CYAN "Blue " S_COLOR_WHITE "leads";
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;

			tmp = va( S_COLOR_CYAN "%i " S_COLOR_WHITE "/ " S_COLOR_RED "%i", cgs.scores2, cgs.scores1 );
			textHeight = fontLarge.Height( tmp );
			textWidth = fontLarge.Width( tmp );
			fontLarge.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWED );
			y += textHeight;
		}

		//TODO: playing until x/y
		break;

	default:
		break;
	}
}

static void DrawPlayerCount_Free( float fade ) {
	const Font font( FONT_JAPPLARGE, cg_newScoreboardFontSize.value, false );
	const char *tmp = NULL;
	float y = 108.0f;
	float textWidth = 0.0f;
	float textHeight = 0.0f;

	// count the free, spectating and bot players
	int freeCount = 0, specCount = 0, botCount = 0;
	for ( int i = 0; i < cg.numScores; i++ ) {
		clientInfo_t &ci = cgs.clientinfo[cg.scores[i].client];
		if ( ci.team == TEAM_FREE ) {
			freeCount++;
		}
		else if ( ci.team == TEAM_SPECTATOR ) {
			specCount++;
		}
		if ( ci.botSkill != -1 ) {
			botCount++;
		}
	}

	// player count
	if ( botCount ) {
		tmp = va( "%i players / %i bots", freeCount - botCount, botCount );
	}
	else {
		tmp = va( "%i players", freeCount );
	}
	textHeight = font.Height( tmp );
	textWidth = font.Width( tmp );
	font.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colorWhite, ITEM_TEXTSTYLE_SHADOWED );
	y += textHeight;

	// spectator count
	tmp = va( "%2i spectators", specCount );
	textHeight = font.Height( tmp );
	textWidth = font.Width( tmp );
	font.Paint( (SCREEN_WIDTH / 2.0f) - textWidth / 2.0f, y, tmp, &colorWhite, ITEM_TEXTSTYLE_SHADOWED );
	y += textHeight;
}

static void DrawPlayerCount_Team( float fade ) {
	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float width = SCREEN_WIDTH / 2.0f;
	const float lineHeight = 14.0f;
	float y = 108.0f;
	const char *tmp = NULL;
	vector4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	int i, redCount = 0, blueCount = 0, specCount = 0, pingAccumRed = 0, pingAvgRed = 0, pingAccumBlue = 0, pingAvgBlue = 0;
	colour.a = fade;

	for ( i = 0; i < cg.numScores; i++ ) {
		if ( cgs.clientinfo[cg.scores[i].client].team == TEAM_RED ) {
			pingAccumRed += cg.scores[i].ping;
			redCount++;
		}
		else if ( cgs.clientinfo[cg.scores[i].client].team == TEAM_BLUE ) {
			blueCount++;
			pingAccumBlue += cg.scores[i].ping;
		}
		else if ( cgs.clientinfo[cg.scores[i].client].team == TEAM_SPECTATOR ) {
			specCount++;
		}
	}

	if ( redCount ) {
		pingAvgRed = ceilf( pingAccumRed / redCount );
	}
	if ( blueCount ) {
		pingAvgBlue = ceilf( pingAccumBlue / blueCount );
	}

	if ( cgs.scores1 >= cgs.scores2 ) {
		// red team
		tmp = va( S_COLOR_RED "%i players " S_COLOR_WHITE "(%3i avg ping)", redCount, pingAvgRed );
		font.Paint( width / 2.0f - font.Width( tmp ) / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
		// blue team
		tmp = va( S_COLOR_CYAN "%i players " S_COLOR_WHITE "(%3i avg ping)", blueCount, pingAvgBlue );
		font.Paint( width + width / 2.0f - font.Width( tmp ) / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
	}
	else {
		tmp = va( S_COLOR_CYAN "%i players " S_COLOR_WHITE "(%i avg ping)", blueCount, pingAvgBlue );
		font.Paint( width / 2.0f - font.Width( tmp ) / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
		tmp = va( S_COLOR_RED "%i players " S_COLOR_WHITE "(%i avg ping)", redCount, pingAvgRed );
		font.Paint( width + width / 2.0f - font.Width( tmp ) / 2.0f, y, tmp, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
	}

	tmp = va( "%2i spectators", specCount );
	font.Paint( width - font.Width( tmp ) / 2.0f, y + lineHeight * 20, tmp, &colour );
}

static void DrawPlayerCount( float fade ) {
	switch ( cgs.gametype ) {
	case GT_FFA:
	case GT_HOLOCRON:
	case GT_JEDIMASTER:
	case GT_DUEL:
	case GT_POWERDUEL:
	case GT_SINGLE_PLAYER:
		DrawPlayerCount_Free( fade );
		break;

	case GT_TEAM:
	case GT_SIEGE:
	case GT_CTF:
	case GT_CTY:
		DrawPlayerCount_Team( fade );
		break;
	default:
		break;
	}
}

// number of players on team 'team'
static int PlayerCount( team_t team ) {
	int count = 0;

	for ( int i = 0; i < cg.numScores; i++ ) {
		const clientInfo_t &ci = cgs.clientinfo[cg.scores[i].client];
		if ( ci.team == team ) {
			count++;
		}
	}

	return count;
}

struct Column {
	const char *title;
	std::function<
		float(
			Column &self, const Font &font
		)
	> CalculateWidth;
	std::function<
		void(
			const Column &self,
			float &x,
			float y,
			const Font &font,
			float fade
		)
	> DrawTitle;
	std::function<
		bool(
			const Column &self,
			float x,
			float y,
			const Font &font,
			float fade,
			const score_t &score,
			const clientInfo_t &ci
		)
	> DrawElement;
	vector2 padding; // left and right padding
	float width;
};

// returns number of players on team 'team'
static int ListPlayers_TDM( float fade, float _x, float _y, team_t team ) {
	const int playerCount = PlayerCount( team );
	if ( !playerCount ) {
		return 0;
	}

	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float startX = _x;//0.0f;
	const float startY = _y;//128.0f;
	float x = startX;
	float y = startY;

	enum ColumnID : uint32_t {
		COLUMN_NAME,
		COLUMN_SCORE,
		COLUMN_RATIO,
		//COLUMN_KPM,
		COLUMN_PING,
		COLUMN_TIME,
		NUM_COLUMNS
	};
	static Column columns[NUM_COLUMNS] = {
		{
			"Name",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( self.title ),
					font.Width( "12345678901234567890123456789012345" )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint( x + self.width - textWidth, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const char *text = ci.name;
				const float textWidth = font.Width( text );
				font.Paint( x + self.width - textWidth, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 0.0f, 4.0f }
		},
		{
			"Score",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "88 / 88" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "Connecting";
				}
				else {
					text = va( "%i / %i ", score.score, score.deaths );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Ratio",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "1.33" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "Connecting";
				}
				else {
					const float ratio = (score.deaths != 0)
						? (static_cast<float>(score.score) / score.deaths)
						: score.score;
					/*
					const int32_t net = score.kills - score.deaths;
					const char netSign = (net >= 0) ? '+' : '-';
					net = math.floor( math.abs( net ) )
					*/

					text = va( "%.2f", ratio );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				return true;
			},
			{ 4.0f, 4.0f }
		},
		#if 0
		{
			"KPM",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "--" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const char *text = "--";
				const float textWidth = font.Width( text );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
				return true;
			},
			{ 4.0f, 4.0f }
		},
		#endif
		{
			"Ping",
			[]( Column &self, const Font &font ) {
				float longestPing = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const clientInfo_t &ci = cgs.clientinfo[score.client];
					const char *text = nullptr;
					if ( ci.botSkill != -1 ) {
						text = "--";
					}
					else {
						text = va( "%i", score.ping );
					}

					const float pingWidth = font.Width( text );
					if ( longestPing < pingWidth ) {
						longestPing = pingWidth;
					}
				}
				return std::max(
					longestPing,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( ci.botSkill != -1 ) {
					const char *text = "--";
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				else {
					vector4 pingColour{ 1.0f, 1.0f, 1.0f, fade };
					const vector4 pingGood{ 0.0f, 1.0f, 0.0f, fade };
					const vector4 pingBad{ 1.0f, 0.0f, 0.0f, fade };
					CG_LerpColour( &pingGood, &pingBad, &pingColour, std::min( score.ping / 300.0f, 1.0f ) );

					const char *text = va( "%i", score.ping );
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &pingColour, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Time",
			[]( Column &self, const Font &font ) {
				float longestTime = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const char *text = va( "%i", score.time );

					const float timeWidth = font.Width( text );
					if ( longestTime < timeWidth ) {
						longestTime = timeWidth;
					}
				}
				return std::max(
					longestTime,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = va( "%i", score.time );
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( text );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
				return true;
			},
			{ 4.0f, 4.0f }
		},
	};

	static int lastFont = -1;
	static float lastScale = 0.0f;
	if ( lastFont != font.handle || !flcmp( lastScale, font.scale ) ) {
		lastFont = font.handle;
		lastScale = font.scale;
		for ( auto &column : columns ) {
			column.width = 0.0f; // force it to recalculate
		}
	}

	// calculate column widths + maximum line height
	float lineHeight = font.Height();
	for ( auto &column : columns ) {
		// we don't ever want the width to reduce in size once it's increased (unless font changes)
		const float width = column.CalculateWidth( column, font );
		if ( column.width < width ) {
			column.width = width;
		}
	}

	for ( auto &column : columns ) {
		x += column.padding.x;
		column.DrawTitle( column, x, y, font, fade );
		y += lineHeight * 1.5f;

		for ( int i = 0; i < cg.numScores; i++ ) {
			const score_t &score = cg.scores[i];
			const clientInfo_t &ci = cgs.clientinfo[score.client];
			if ( !ci.infoValid || ci.team != team ) {
				continue;
			}
			if ( column.DrawElement( column, x, y, font, fade, score, ci ) ) {
				y += lineHeight;
			}
		}
		x += column.width + column.padding.y;
		y = startY;
	}
	x = startX;

#if 0
	// Second pass, text + icons
	for ( int i = 0; i < playerCount; i++ ) {
		score_t *score = &cg.scores[i];
		clientInfo_t *ci = &cgs.clientinfo[score->client];
		if ( ci->team == TEAM_FREE ) {
			if ( score->ping >= 999 || (cg_entities[score->client].currentState.eFlags & EF_CONNECTION) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.connection );
				trap->R_SetColor( NULL );
			}

			else if ( cg.snap->ps.duelInProgress && (ci - cgs.clientinfo == cg.snap->ps.duelIndex
				|| ci - cgs.clientinfo == cg.snap->ps.clientNum) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.powerduelAlly );
				trap->R_SetColor( NULL );
			}

			else if ( ci->botSkill != -1 ) {
				tmp = "BOT";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}

			else if ( cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client) ) {
				tmp = "READY";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}
		}
	}
#endif

	return 0;
}

// returns number of players on team 'team'
static int ListPlayers_CTF( float fade, float _x, float _y, team_t team ) {
	const int playerCount = PlayerCount( team );
	if ( !playerCount ) {
		return 0;
	}

	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float startX = _x;//0.0f;
	const float startY = _y;//128.0f;
	float x = startX;
	float y = startY;

	enum ColumnID : uint32_t {
		COLUMN_NAME,
		COLUMN_SCORE,
		COLUMN_CAPTURES,
		COLUMN_ASSISTS,
		COLUMN_DEFENSE,
		COLUMN_PING,
		COLUMN_TIME,
		NUM_COLUMNS
	};
	static Column columns[NUM_COLUMNS] = {
		{
			"Name",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( self.title ),
					font.Width( "12345678901234567890123456789012345" )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint( x + self.width - textWidth, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const char *text = ci.name;
				const float textWidth = font.Width( text );
				font.Paint( x + self.width - textWidth, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 0.0f, 4.0f }
		},
		{
			"Score",
			[]( Column &self, const Font &font ) {
				return std::max(
					std::max(
						font.Width( "Connecting" ),
						font.Width( "8888" )
					),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "Connecting";
				}
				else {
					text = va( "%i", score.score );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Captures",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "88" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "--";
				}
				else {
					text = va( "%i", score.captures );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Assists",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "88" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "--";
				}
				else {
					text = va( "%i", score.assistCount );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Defense",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( "88" ),
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "--";
				}
				else {
					text = va( "%i", score.defendCount );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Ping",
			[]( Column &self, const Font &font ) {
				float longestPing = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const clientInfo_t &ci = cgs.clientinfo[score.client];
					const char *text = nullptr;
					if ( ci.botSkill != -1 ) {
						text = "--";
					}
					else {
						text = va( "%i", score.ping );
					}

					const float pingWidth = font.Width( text );
					if ( longestPing < pingWidth ) {
						longestPing = pingWidth;
					}
				}
				return std::max(
					longestPing,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( ci.botSkill != -1 ) {
					const char *text = "--";
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				else {
					vector4 pingColour{ 1.0f, 1.0f, 1.0f, fade };
					const vector4 pingGood{ 0.0f, 1.0f, 0.0f, fade };
					const vector4 pingBad{ 1.0f, 0.0f, 0.0f, fade };
					CG_LerpColour( &pingGood, &pingBad, &pingColour, std::min( score.ping / 300.0f, 1.0f ) );

					const char *text = va( "%i", score.ping );
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &pingColour, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Time",
			[]( Column &self, const Font &font ) {
				float longestTime = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const char *text = va( "%i", score.time );

					const float timeWidth = font.Width( text );
					if ( longestTime < timeWidth ) {
						longestTime = timeWidth;
					}
				}
				return std::max(
					longestTime,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = va( "%i", score.time );
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( text );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
				return true;
			},
			{ 4.0f, 4.0f }
		},
	};

	static int lastFont = -1;
	static float lastScale = 0.0f;
	if ( lastFont != font.handle || !flcmp( lastScale, font.scale ) ) {
		lastFont = font.handle;
		lastScale = font.scale;
		for ( auto &column : columns ) {
			column.width = 0.0f; // force it to recalculate
		}
	}

	// calculate column widths + maximum line height
	float lineHeight = font.Height();
	for ( auto &column : columns ) {
		// we don't ever want the width to reduce in size once it's increased (unless font changes)
		const float width = column.CalculateWidth( column, font );
		if ( column.width < width ) {
			column.width = width;
		}
	}

	for ( auto &column : columns ) {
		x += column.padding.x;
		column.DrawTitle( column, x, y, font, fade );
		y += lineHeight * 1.5f;

		for ( int i = 0; i < cg.numScores; i++ ) {
			const score_t &score = cg.scores[i];
			const clientInfo_t &ci = cgs.clientinfo[score.client];
			if ( !ci.infoValid || ci.team != team ) {
				continue;
			}
			if ( column.DrawElement( column, x, y, font, fade, score, ci ) ) {
				y += lineHeight;
			}
		}
		x += column.width + column.padding.y;
		y = startY;
	}
	x = startX;

#if 0
	// Second pass, text + icons
	for ( int i = 0; i < playerCount; i++ ) {
		score_t *score = &cg.scores[i];
		clientInfo_t *ci = &cgs.clientinfo[score->client];
		if ( ci->team == TEAM_FREE ) {
			if ( score->ping >= 999 || (cg_entities[score->client].currentState.eFlags & EF_CONNECTION) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.connection );
				trap->R_SetColor( NULL );
			}

			else if ( cg.snap->ps.duelInProgress && (ci - cgs.clientinfo == cg.snap->ps.duelIndex
				|| ci - cgs.clientinfo == cg.snap->ps.clientNum) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.powerduelAlly );
				trap->R_SetColor( NULL );
			}

			else if ( ci->botSkill != -1 ) {
				tmp = "BOT";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}

			else if ( cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client) ) {
				tmp = "READY";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}
		}
	}
#endif

	return 0;
}

// render a list of players on team 'team' at 'x', 'y' using relevant information based on gametype
static int ListPlayers_Team( float fade, float x, float y, team_t team ) {
	switch ( cgs.gametype ) {
	case GT_FFA:
	case GT_HOLOCRON:
	case GT_JEDIMASTER:
	case GT_DUEL:
	case GT_POWERDUEL:
	case GT_SINGLE_PLAYER:
		trap->Error( ERR_DROP, "Tried to use team scoreboard on non-team gametype" );
		break;

	case GT_TEAM:
		return ListPlayers_TDM( fade, x, y, team );

	case GT_SIEGE:
		break;

	case GT_CTF:
	case GT_CTY:
		return ListPlayers_CTF( fade, x, y, team );

	default:
		break;
	}

	return -1;
}

static void DrawSpectators( float fade ) {
	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float lineHeight = 14.0f;
	float y = 128.0f;
	vector4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
	white.a = fade;

	CG_BuildSpectatorString();
	cg.scoreboard.spectatorWidth = font.Width( cg.scoreboard.spectatorList );

	if ( cg.scoreboard.spectatorLen ) {
		const float dt = (cg.time - cg.scoreboard.spectatorResetTime)*0.0625f;
		cg.scoreboard.spectatorX = SCREEN_WIDTH - (1.0f)*dt;

		if ( cg.scoreboard.spectatorX < 0 - cg.scoreboard.spectatorWidth ) {
			cg.scoreboard.spectatorX = SCREEN_WIDTH;
			cg.scoreboard.spectatorResetTime = cg.time;
		}
		font.Paint(
			cg.scoreboard.spectatorX, (y + lineHeight * 20) - 3, cg.scoreboard.spectatorList, &white,
			ITEM_TEXTSTYLE_SHADOWED
		);
	}
}

static void DrawPlayers_Free( float fade ) {
	const int playerCount = PlayerCount( TEAM_FREE );
	if ( !playerCount ) {
		return;
	}

	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float startX = 0.0f;
	const float startY = 128.0f;
	float x = startX;
	float y = startY;

	enum ColumnID : uint32_t {
		COLUMN_NAME,
		COLUMN_SCORE,
		COLUMN_PING,
		COLUMN_TIME,
		NUM_COLUMNS
	};
	static Column columns[NUM_COLUMNS] = {
		{
			"Name",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( self.title ),
					font.Width( "12345678901234567890123456789012345" )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint( x + self.width - textWidth, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const char *text = ci.name;
				const float textWidth = font.Width( text );
				font.Paint( x + self.width - textWidth, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 0.0f, 4.0f }
		},
		{
			"Score",
			[]( Column &self, const Font &font ) {
				float longestScore = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const clientInfo_t &ci = cgs.clientinfo[score.client];
					if ( !ci.infoValid || ci.team != TEAM_FREE ) {
						continue;
					}
					const char *text = nullptr;
					if ( score.ping == -1 ) {
						text = "Connecting";
					}
					else if ( cg_drawScoresNet.integer ) {
						const int net = score.score - score.deaths;
						text = va( "%02i/%02i (%c%i)", score.score, score.deaths, (net >= 0) ? '+' : '-', abs( net ) );
					}
					else {
						text = va( "%02i/%02i", score.score, score.deaths );
					}
					const float scoreWidth = font.Width( text );
					if ( longestScore < scoreWidth ) {
						longestScore = scoreWidth;
					}
				}
				return std::max(
					longestScore,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = nullptr;
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( score.ping == -1 ) {
					text = "Connecting";
				}
				/*
				else if ( team == TEAM_SPECTATOR ) {
					text = "Spectating"; //TODO: Name of client they're spectating? possible?
				}
				*/
				else if ( cg_drawScoresNet.integer ) {
					const int net = score.score - score.deaths;
					text = va( "%02i/%02i (%c%i)", score.score, score.deaths, (net >= 0) ? '+' : '-', abs( net ) );
				}
				else {
					text = va( "%02i/%02i", score.score, score.deaths );
				}
				font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				return true;
			},
			{ 4.0f, 4.0f }
		},
		//TODO: deaths, net, kpm
		{
			"Ping",
			[]( Column &self, const Font &font ) {
				float longestPing = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const clientInfo_t &ci = cgs.clientinfo[score.client];
					const char *text = nullptr;
					if ( ci.botSkill != -1 ) {
						text = "--";
					}
					else {
						text = va( "%i", score.ping );
					}

					const float pingWidth = font.Width( text );
					if ( longestPing < pingWidth ) {
						longestPing = pingWidth;
					}
				}
				return std::max(
					longestPing,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( ci.botSkill != -1 ) {
					const char *text = "--";
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				else {
					vector4 pingColour{ 1.0f, 1.0f, 1.0f, fade };
					const vector4 pingGood{ 0.0f, 1.0f, 0.0f, fade };
					const vector4 pingBad{ 1.0f, 0.0f, 0.0f, fade };
					CG_LerpColour( &pingGood, &pingBad, &pingColour, std::min( score.ping / 300.0f, 1.0f ) );

					const char *text = va( "%i", score.ping );
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &pingColour, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Time",
			[]( Column &self, const Font &font ) {
				float longestTime = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const char *text = va( "%i", score.time );

					const float timeWidth = font.Width( text );
					if ( longestTime < timeWidth ) {
						longestTime = timeWidth;
					}
				}
				return std::max(
					longestTime,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = va( "%i", score.time );
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( text );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
				return true;
			},
			{ 4.0f, 4.0f }
		},
	};

	static int lastFont = -1;
	static float lastScale = 0.0f;
	if ( lastFont != font.handle || !flcmp( lastScale, font.scale ) ) {
		lastFont = font.handle;
		lastScale = font.scale;
		for ( auto &column : columns ) {
			column.width = 0.0f; // force it to recalculate
		}
	}

	// calculate column widths + maximum line height
	float lineHeight = font.Height();
	for ( auto &column : columns ) {
		// we don't ever want the width to reduce in size once it's increased (unless font changes)
		const float width = column.CalculateWidth( column, font );
		if ( column.width < width ) {
			column.width = width;
		}
	}

	for ( auto &column : columns ) {
		x += column.padding.x;
		column.DrawTitle( column, x, y, font, fade );
		y += lineHeight * 1.5f;

		for ( int i = 0; i < cg.numScores; i++ ) {
			const score_t &score = cg.scores[i];
			const clientInfo_t &ci = cgs.clientinfo[score.client];
			if ( !ci.infoValid || ci.team != TEAM_FREE ) {
				continue;
			}
			if ( column.DrawElement( column, x, y, font, fade, score, ci ) ) {
				y += lineHeight;
			}
		}
		x += column.width + column.padding.y;
		y = startY;
	}
	x = startX;

#if 0
	// Second pass, text + icons
	for ( int i = 0; i < playerCount; i++ ) {
		score_t *score = &cg.scores[i];
		clientInfo_t *ci = &cgs.clientinfo[score->client];
		if ( ci->team == TEAM_FREE ) {
			if ( score->ping >= 999 || (cg_entities[score->client].currentState.eFlags & EF_CONNECTION) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.connection );
				trap->R_SetColor( NULL );
			}

			else if ( cg.snap->ps.duelInProgress && (ci - cgs.clientinfo == cg.snap->ps.duelIndex
				|| ci - cgs.clientinfo == cg.snap->ps.clientNum) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.powerduelAlly );
				trap->R_SetColor( NULL );
			}

			else if ( ci->botSkill != -1 ) {
				tmp = "BOT";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}

			else if ( cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client) ) {
				tmp = "READY";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}
		}
	}
#endif

	DrawSpectators( fade );
}

static void DrawPlayers_Duel( float fade ) {
	const int playerCount = PlayerCount( TEAM_FREE );
	if ( !playerCount ) {
		return;
	}

	const Font font( JP_GetScoreboardFont(), cg_newScoreboardFontSize.value, false );
	const float startX = 0.0f;
	const float startY = 128.0f;
	float x = startX;
	float y = startY;

	enum ColumnID : uint32_t {
		COLUMN_NAME,
		COLUMN_SCORE,
		COLUMN_PROGRESS,
		COLUMN_PING,
		COLUMN_TIME,
		NUM_COLUMNS
	};
	static Column columns[NUM_COLUMNS] = {
		{
			"Name",
			[]( Column &self, const Font &font ) {
				return std::max(
					font.Width( self.title ),
					font.Width( "12345678901234567890123456789012345" )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint( x + self.width - textWidth, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const char *text = ci.name;
				const float textWidth = font.Width( text );
				font.Paint( x + self.width - textWidth, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );

				return true;
			},
			{ 0.0f, 4.0f }
		},
		{
			"Game",
			[]( Column &self, const Font &font ) {
				return 128;
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const int32_t numBlocks = cgs.fraglimit;
				real32_t blockWidth = self.width / numBlocks;
				const real32_t nmd = Q_RoundToNearMultipleDown( blockWidth, 8 );
				if ( nmd != 0 ) {
					blockWidth = nmd;
				}
				const real32_t blockHeight = font.Height();
				int32_t usedBlocks = 0;

				/*
				// draw deaths first
				const int32_t numLossBlocks = std::min( score.deaths, cgs.fraglimit );
				for ( int32_t i = 0; i < numLossBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_RED] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}
				*/

				// then kills
				const int32_t numKillBlocks = score.score - usedBlocks;
				for ( int32_t i = 0; i < numKillBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_GREEN] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}

				// pad with grey blocks
				const int32_t numRemainingBlocks = numBlocks - usedBlocks;
				for ( int32_t i = 0; i < numRemainingBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_DKGREY] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}

				#if 0
					const char *text = nullptr;
					const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
					if ( score.ping == -1 ) {
						text = "Connecting";
					}
					/*
					else if ( team == TEAM_SPECTATOR ) {
						text = "Spectating"; //TODO: Name of client they're spectating? possible?
					}
					*/
					else {
						text = va( "%02i of %02i " S_COLOR_GREY "(L: %02i)", ci.wins, cgs.duel_fraglimit, ci.losses );
					}
					font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				#endif
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Match",
			[]( Column &self, const Font &font ) {
				return 128;
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				font.Paint( x, y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const int32_t numBlocks = cgs.duel_fraglimit;
				real32_t blockWidth = self.width / numBlocks;
				const real32_t nmd = Q_RoundToNearMultipleDown( blockWidth, 8 );
				if ( nmd != 0 ) {
					blockWidth = nmd;
				}
				const real32_t blockHeight = font.Height();
				int32_t usedBlocks = 0;

				/*
				// draw losses first
				const int32_t numLossBlocks = std::min( ci.losses, cgs.duel_fraglimit );
				for ( int32_t i = 0; i < numLossBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_RED] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}
				*/

				// then wins
				const int32_t numWinBlocks = ci.wins - usedBlocks;
				for ( int32_t i = 0; i < numWinBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_GREEN] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}

				// pad with grey blocks
				const int32_t numRemainingBlocks = numBlocks - usedBlocks;
				for ( int32_t i = 0; i < numRemainingBlocks; i++ ) {
					CG_FillRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, &colorTable[CT_DKGREY] );
					CG_DrawRect( x + (usedBlocks * blockWidth), y, blockWidth, blockHeight, 0.25f, &colorTable[CT_BLACK] );
					usedBlocks++;
				}

				#if 0
					const char *text = nullptr;
					const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
					if ( score.ping == -1 ) {
						text = "Connecting";
					}
					/*
					else if ( team == TEAM_SPECTATOR ) {
						text = "Spectating"; //TODO: Name of client they're spectating? possible?
					}
					*/
					else {
						text = va( "%02i of %02i " S_COLOR_GREY "(L: %02i)", ci.wins, cgs.duel_fraglimit, ci.losses );
					}
					font.Paint( x, y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
				#endif
				return true;
			},
			{ 4.0f, 4.0f }
		},
		//TODO: deaths, net, kpm
		{
			"Ping",
			[]( Column &self, const Font &font ) {
				float longestPing = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const clientInfo_t &ci = cgs.clientinfo[score.client];
					const char *text = nullptr;
					if ( ci.botSkill != -1 ) {
						text = "--";
					}
					else {
						text = va( "%i", score.ping );
					}

					const float pingWidth = font.Width( text );
					if ( longestPing < pingWidth ) {
						longestPing = pingWidth;
					}
				}
				return std::max(
					longestPing,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				if ( ci.botSkill != -1 ) {
					const char *text = "--";
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				else {
					vector4 pingColour{ 1.0f, 1.0f, 1.0f, fade };
					const vector4 pingGood{ 0.0f, 1.0f, 0.0f, fade };
					const vector4 pingBad{ 1.0f, 0.0f, 0.0f, fade };
					CG_LerpColour( &pingGood, &pingBad, &pingColour, std::min( score.ping / 300.0f, 1.0f ) );

					const char *text = va( "%i", score.ping );
					const float textWidth = font.Width( text );
					font.Paint(
						x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &pingColour, ITEM_TEXTSTYLE_SHADOWEDMORE
					);
				}
				return true;
			},
			{ 4.0f, 4.0f }
		},
		{
			"Time",
			[]( Column &self, const Font &font ) {
				float longestTime = 0.0f;
				for ( int i = 0; i < cg.numScores; i++ ) {
					const score_t &score = cg.scores[i];
					const char *text = va( "%i", score.time );

					const float timeWidth = font.Width( text );
					if ( longestTime < timeWidth ) {
						longestTime = timeWidth;
					}
				}
				return std::max(
					longestTime,
					font.Width( self.title )
				);
			},
			[]( const Column &self, float &x, float y, const Font &font, float fade ) {
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( self.title );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, self.title, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
			},
			[]( const Column &self, float x, float y, const Font &font, float fade, const score_t &score,
				const clientInfo_t &ci )
			{
				const char *text = va( "%i", score.time );
				const vector4 white{ 1.0f, 1.0f, 1.0f, fade };
				const float textWidth = font.Width( text );
				font.Paint(
					x + (self.width / 2.0f) - (textWidth / 2.0f), y, text, &white, ITEM_TEXTSTYLE_SHADOWEDMORE
				);
				return true;
			},
			{ 4.0f, 4.0f }
		},
	};

	static int lastFont = -1;
	static float lastScale = 0.0f;
	if ( lastFont != font.handle || !flcmp( lastScale, font.scale ) ) {
		lastFont = font.handle;
		lastScale = font.scale;
		for ( auto &column : columns ) {
			column.width = 0.0f; // force it to recalculate
		}
	}

	// calculate column widths + maximum line height
	float lineHeight = font.Height();
	for ( auto &column : columns ) {
		// we don't ever want the width to reduce in size once it's increased (unless font changes)
		const float width = column.CalculateWidth( column, font );
		if ( column.width < width ) {
			column.width = width;
		}
	}

	for ( auto &column : columns ) {
		x += column.padding.x;
		column.DrawTitle( column, x, y, font, fade );
		y += lineHeight * 1.5f;

		for ( int i = 0; i < cg.numScores; i++ ) {
			const score_t &score = cg.scores[i];
			const clientInfo_t &ci = cgs.clientinfo[score.client];
			if ( !ci.infoValid || ci.team != TEAM_FREE ) {
				continue;
			}
			if ( column.DrawElement( column, x, y, font, fade, score, ci ) ) {
				y += lineHeight;
			}
		}
		x += column.width + column.padding.y;
		y = startY;
	}
	x = startX;

#if 0
	// Second pass, text + icons
	for ( int i = 0; i < playerCount; i++ ) {
		score_t *score = &cg.scores[i];
		clientInfo_t *ci = &cgs.clientinfo[score->client];
		if ( ci->team == TEAM_FREE ) {
			if ( score->ping >= 999 || (cg_entities[score->client].currentState.eFlags & EF_CONNECTION) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.connection );
				trap->R_SetColor( NULL );
			}

			else if ( cg.snap->ps.duelInProgress && (ci - cgs.clientinfo == cg.snap->ps.duelIndex
				|| ci - cgs.clientinfo == cg.snap->ps.clientNum) ) {
				trap->R_SetColor( &white );
				CG_DrawPic( x + 5.0f, y + 1.0f, lineHeight - 2.0f, lineHeight - 2.0f, media.gfx.interface.powerduelAlly );
				trap->R_SetColor( NULL );
			}

			else if ( ci->botSkill != -1 ) {
				tmp = "BOT";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}

			else if ( cg.snap->ps.stats[STAT_CLIENTS_READY] & (1 << score->client) ) {
				tmp = "READY";
				font.Paint( x + 8.0f, y + (font.Height( tmp ) / 2.0f), tmp, &white, ITEM_TEXTSTYLE_SHADOWEDMORE );
			}
		}
	}
#endif

	DrawSpectators( fade );
}

static void DrawPlayers_Team( float fade ) {
	const float y = 128.0f;

	if ( cgs.scores1 >= cgs.scores2 ) {
		ListPlayers_Team( fade, 0, y, TEAM_RED );
		ListPlayers_Team( fade, 0 + SCREEN_WIDTH / 2.0f, y, TEAM_BLUE );
	}
	else {
		ListPlayers_Team( fade, 0, y, TEAM_BLUE );
		ListPlayers_Team( fade, 0 + SCREEN_WIDTH / 2.0f, y, TEAM_RED );
	}

	DrawSpectators( fade );
}

// controller for drawing the 'players' section. will call the relevant functions based on gametype
static void DrawPlayers( float fade ) {
	switch ( cgs.gametype ) {
	case GT_FFA:
	case GT_HOLOCRON:
	case GT_JEDIMASTER:
	case GT_SINGLE_PLAYER:
		DrawPlayers_Free( fade );
		break;

	case GT_DUEL:
	case GT_POWERDUEL:
		DrawPlayers_Duel( fade );
		break;

	case GT_TEAM:
	case GT_SIEGE:
	case GT_CTF:
	case GT_CTY:
		DrawPlayers_Team( fade );
		break;

	default:
		break;
	}
}

static const char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec" };

// e.g. #st, #nd, #rd, #th
static const char *GetDateSuffix( int date ) {
	switch ( date % 10 ) {
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}

// shows current date and JA++ version
static void DrawClientInfo( float fade ) {
	struct tm *timeinfo;
	time_t tm;
	char buf[256];
	const Font font( FONT_JAPPMONO, 0.5f, false );
	vector4 colour;
	float y = SCREEN_HEIGHT - 4.0f;

	VectorCopy4( &g_color_table[ColorIndex( COLOR_ORANGE )], &colour );
	colour.a = fade;

#ifdef REVISION
	y -= font.Height( REVISION );
	// JA++ version
	font.Paint( SCREEN_WIDTH - font.Width( REVISION ), y, REVISION, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
#endif

	// date
	time( &tm );
	timeinfo = localtime( &tm );

	Com_sprintf( buf, sizeof(buf), "%s %i%s %04i, %02i:%02i:%02i",
		months[timeinfo->tm_mon], timeinfo->tm_mday, GetDateSuffix( timeinfo->tm_mday ),
		1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec
	);

	y -= font.Height( buf );
	font.Paint( SCREEN_WIDTH - font.Width( buf ), y, buf, &colour, ITEM_TEXTSTYLE_SHADOWEDMORE );
}

//Scoreboard entry point
//	This will be called even if the scoreboard is not showing - and will return false if it has faded aka 'not showing'
//	It will return true if the scoreboard is showing
qboolean CG_DrawJAPPScoreboard( void ) {
	vector4 fadeWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fade = 1.0f;

	if ( cg.warmup && !cg.showScores )
		return qfalse;

	if ( !cg.showScores && cg.snap->ps.pm_type != PM_DEAD && cg.snap->ps.pm_type != PM_INTERMISSION ) {
		if ( CG_FadeColor2( &fadeWhite, cg.scoreFadeTime, 300.0f ) )
			return qfalse;

		fade = fadeWhite.a;
	}

	DrawServerInfo( fade );

	DrawPlayerCount( fade );
	DrawPlayers( fade );

	DrawClientInfo( fade );

	CG_LoadDeferredPlayers();
	return qtrue;
}
