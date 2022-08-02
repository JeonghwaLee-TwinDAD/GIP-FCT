USE [lunardb]
GO
ALTER TABLE [dbo].[Limits] DROP CONSTRAINT [DF_Limits_TimeStamp]
GO
ALTER TABLE [dbo].[Limits] DROP CONSTRAINT [DF_Limits_ID]
GO
/****** Object:  Table [dbo].[Partnumber]    Script Date: 4/12/2022 2:36:49 PM ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Partnumber]') AND type in (N'U'))
DROP TABLE [dbo].[Partnumber]
GO
/****** Object:  Table [dbo].[Limits]    Script Date: 4/12/2022 2:36:49 PM ******/
IF  EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[Limits]') AND type in (N'U'))
DROP TABLE [dbo].[Limits]
GO
/****** Object:  Table [dbo].[Limits]    Script Date: 4/12/2022 2:36:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Limits](
	[ID] [uniqueidentifier] NOT NULL,
	[TimeStamp] [datetime] NOT NULL,
	[ErrorCodes] [varchar](50) NOT NULL,
	[LowLimit] [numeric](18, 3) NULL,
	[HighLimit] [numeric](18, 3) NULL,
	[Comparator] [varchar](50) NULL,
	[Units] [varchar](10) NULL,
	[StringComp] [varchar](255) NULL,
	[Models] [varchar](50) NOT NULL,
	[Description] [varchar](50) NULL,
 CONSTRAINT [PK_Limits] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Partnumber]    Script Date: 4/12/2022 2:36:49 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Partnumber](
	[ID] [uniqueidentifier] NOT NULL,
	[PartNumberRegex] [varchar](255) NOT NULL,
	[Model] [varchar](10) NOT NULL,
	[Suffix] [varchar](10) NOT NULL,
 CONSTRAINT [PK_Partnumber] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'31b3f1ab-26d9-48fa-bd50-1b5b0bd58541', CAST(N'2021-12-21T14:20:02.430' AS DateTime), N'LimitCode2', CAST(10.000 AS Numeric(18, 3)), CAST(20.000 AS Numeric(18, 3)), N'GELE', N'Amps', NULL, N'SubM,0B,0C', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'7a211bf4-7a7f-49bb-b82a-642392515184', CAST(N'2022-03-02T12:31:17.930' AS DateTime), N'A1231233', NULL, NULL, NULL, NULL, NULL, N'03', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'6974b6ec-368b-4157-8250-6ea3170a4750', CAST(N'2021-12-21T14:17:50.983' AS DateTime), N'Sample1', CAST(5.000 AS Numeric(18, 3)), CAST(15.000 AS Numeric(18, 3)), N'GTLE', N'Vdc', NULL, N'SubM', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'b3fe3c6a-c067-473d-a9f5-8f66ab39582e', CAST(N'2021-12-21T14:17:12.400' AS DateTime), N'A0001243', CAST(5.000 AS Numeric(18, 3)), CAST(10.000 AS Numeric(18, 3)), N'GELE', N'C', NULL, N'SubM,0C', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'7fcc4ead-ebc2-41b7-9084-b5d14eafeeee', CAST(N'2021-12-21T14:20:30.533' AS DateTime), N'A0001243', CAST(10.000 AS Numeric(18, 3)), CAST(20.000 AS Numeric(18, 3)), N'GELE', N'C', NULL, N'0C,0B', NULL)
INSERT [dbo].[Limits] ([ID], [TimeStamp], [ErrorCodes], [LowLimit], [HighLimit], [Comparator], [Units], [StringComp], [Models], [Description]) VALUES (N'ad0af5c2-2fe7-4ff1-a8e1-c0c5faf8b20d', CAST(N'2021-12-21T14:57:14.250' AS DateTime), N'A0001243', CAST(5.000 AS Numeric(18, 3)), CAST(10.000 AS Numeric(18, 3)), N'GELE', N'C', NULL, N'SubM', NULL)
GO
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'fa4f72a4-5e99-4093-956d-40a0ba740e36', N'^(P:)(\w{2}-\w{6})(R:)(\w{2})(S:)((\w{3})(\d{2})(\d{2})(\d{5}))$', N'SubM', N'B1  ')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'54d08a76-3c8c-49c7-9978-941d39cdbd69', N'^(S:\w{3}\d{2}\d{2}(\d{4}|[[:xdigit:]]{4}))(P:(\d{8})(\w{1,2})(\d{6})(\d{3}))$', N'0C', N'A2  ')
INSERT [dbo].[Partnumber] ([ID], [PartNumberRegex], [Model], [Suffix]) VALUES (N'a382ead5-775e-4515-9346-9721282186ab', N'^J(\d{11,13}):K(\d{11,13})$', N'0B', N'A1  ')
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_ID]  DEFAULT (newid()) FOR [ID]
GO
ALTER TABLE [dbo].[Limits] ADD  CONSTRAINT [DF_Limits_TimeStamp]  DEFAULT (getdate()) FOR [TimeStamp]
GO
